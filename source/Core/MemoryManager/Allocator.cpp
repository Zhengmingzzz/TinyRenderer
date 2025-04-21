//
// Created by Administrator on 25-4-15.
//

#include "Allocator.h"
#include "Page.h"
#include "Function/Message/Message.h"
#include "MemoryManager.h"
#include "Function/Counter/CounterManager.h"

namespace TinyRenderer {
    void Allocator::startUp(size_t block_size,size_t min_blockNum_perPage, size_t max_blockNum_perPage) {
        ASSERT(block_size>0);
        ASSERT(max_blockNum_perPage>0);

        block_size_ = block_size;
        max_blockNum_perPage_ = max_blockNum_perPage;
        current_blockNum_perPage_ = 2;
        min_blockNum_perPage_ = min_blockNum_perPage;
        pagehead_ = nullptr;
        pagetail_ = nullptr;
        ewma_longterm_activity_ = 0.f;
        ewma_longterm_activity_factor_ = 0.7f;

        ewma_shortterm_activity_ = 0.f;
        ewma_shortterm_activity_factor_ = 0.3f;
    }

    void Allocator::shutDown() {
        Page* cur_page = nullptr, *next_page = nullptr;
        for (cur_page = pagehead_; cur_page != nullptr; ) {
            cur_page->shutDown();
            next_page = cur_page->next;
            free(cur_page);
            cur_page = next_page;
        }
    }

    void* Allocator::allocate() {
        //std::lock_guard mutex_lock(allocator_mutex_);
        StopWatch_Start(mem_total_allocation_time);
        void* res = nullptr;
        Page* cur_page = pagehead_;

        while (res == nullptr) {
            // 1. 判断是否还有空的块
            if (cur_page == nullptr) {
                // 双重检测锁，创建新页
                StopWatch_Start(mem_alloc_newPage_time);
                Counter_Add(mem_alloc_newPage_cnt);
                Page* new_page = Page::allocate_newPage(block_size_, static_cast<int>(current_blockNum_perPage_ + 0.9), this);
                StopWatch_Pause(mem_alloc_newPage_time);
                if (pagehead_ == nullptr) {
                    pagehead_ = new_page;
                    pagetail_ = new_page;
                }
                else {
                    pagetail_->next = new_page;
                    pagetail_ = new_page;
                }
                cur_page = new_page;
            }

            // TODO:此处可能会有竞态条件，当删除了其中一个page后，可能会读取到脏数据
            // 2. 分配含有空余块的页，没有则遍历到下一个页
            Counter_Add(mem_try_allocate_block_cnt);
            // TODO:采用小顶堆存放Page减少遍历次数，使用数组存放块数为零的Page。
            if (cur_page->try_allocate_block(res)) {
                new_count_ += 1;
                break;
            }
            cur_page = cur_page->next;
        }
        StopWatch_Pause(mem_total_allocation_time);
        return res;
    }

    bool Allocator::try_dellocate(Page* target_page, Block* target_block, bool& isRecycle_page) {
        ASSERT(target_page!=nullptr);
        ASSERT(target_block!=nullptr);

        //std::lock_guard lock(allocator_mutex_);

        bool is_free = true; // 是否对ptr使用free
        if (target_page->try_deallocate_block(target_block)) {
            delete_count_ += 1;
            is_free = false;

            // 如果页为空了，根据b_isRecycle判断是否要回收页
                if (target_page->current_block_num_ == target_page->total_block_num_) {
                    // 此时可以收回页
                    if (b_isRecycle_) {
                        // 如果要删除头结点，则更新头结点为当前节点的下一个节点
                        if (target_page == pagehead_) {
                            pagehead_ = target_page->next; // 处理头节点的情况
                            if (target_page == pagetail_) {
                                pagetail_ = nullptr;    // 处理既是头结点又是尾结点的情况
                            }
                        }
                        // 处理中间节点的情况
                        else {
                            Page* prev = pagehead_;
                            while (prev != nullptr && prev->next != target_page) {
                                prev = prev->next;
                            }
                            ASSERT(prev!=nullptr);
                            if (prev) { // 找到前驱节点
                                prev->next = target_page->next;
                                if (pagetail_ == target_page) { // 更新尾指针
                                    pagetail_ = prev;
                                }
                            }
                        }


                        // 释放这个页
                        free(target_page);
                        // 通知MemoryManager已经完成页的释放
                        isRecycle_page = true;
                    }
                }

        }
        // 返回块是否被收回
        return !is_free;
    }

    void Allocator::tick() {
        float difference = new_count_ - delete_count_;
        // 如果当前没有new，没有delete，默认减少部分页
        if (new_count_ == 0 && delete_count_ == 0) {
            difference = 0.95f;
        }
        ewma_longterm_activity_ = (ewma_longterm_activity_ * ewma_longterm_activity_factor_) + difference * (1 - ewma_longterm_activity_factor_);
        ewma_shortterm_activity_ = (ewma_shortterm_activity_ * ewma_shortterm_activity_factor_) + difference * (1 - ewma_shortterm_activity_factor_);
        new_count_ = 0;
        delete_count_ = 0;
    }

    void Allocator::refreshMetrics(Occupancy_level occupancy_level) {
        if (occupancy_level == Occupancy_level::low) {
            // 最大/最小可以缩放为原来页的多少倍
            max_longterm_activity_ = 2.f;
            min_longterm_activity_ = 0.7f;
            max_shortterm_activity_ = 1.5f;
            min_shortterm_activity_ = 0.8f;
            // 页达到扩张/缩放条件的阈值
            expand_threshold_ = 0.8f;
            shrink_threshold_ = 0.6f;
        }
        else if (occupancy_level == Occupancy_level::medium) {
            // 最大/最小可以缩放为原来页的多少倍
            max_longterm_activity_ = 1.5f;
            min_longterm_activity_ = 0.7f;
            max_shortterm_activity_ = 1.1f;
            min_shortterm_activity_ = 0.9f;
            // 页达到扩张/缩放条件的阈值
            expand_threshold_ = 1.2f;
            shrink_threshold_ = 0.4f;
        }
        else if (occupancy_level == Occupancy_level::high) {
            // 最大/最小可以缩放为原来页的多少倍
            max_longterm_activity_ = 1.2f;
            min_longterm_activity_ = 0.3f;
            max_shortterm_activity_ = 1.f;
            min_shortterm_activity_ = 0.8f;
            // 页达到扩张/缩放条件的阈值
            expand_threshold_ = 1.5;
            shrink_threshold_ = 0.2f;
        }

        float longterm_factor = 0, shortterm_factor = 0;
        // 页的分配大小以长期活跃值为主，短期活跃值用于根据当前情况微调
        float current_blockNum_perPage = static_cast<float>(current_blockNum_perPage_);
        // 当前长期活跃值大于扩张的边界
        if (ewma_longterm_activity_ > current_blockNum_perPage * expand_threshold_) {
            // 增加每页数量，但是限制其最大上限
            longterm_factor = std::min(ewma_longterm_activity_, max_longterm_activity_);
        }
        // 当前长期活跃值小于缩小的边界
        else if (ewma_longterm_activity_ < current_blockNum_perPage * shrink_threshold_) {
            // 缩小每页数量，但是限制其最小下限
            longterm_factor = std::max(ewma_longterm_activity_, min_longterm_activity_);
        }
        // 当前短期活跃值大于扩张的边界
        if (ewma_shortterm_activity_ > current_blockNum_perPage * expand_threshold_) {
            shortterm_factor = std::min(ewma_shortterm_activity_, max_shortterm_activity_);
        }
        // 当前短期活跃值小于缩小的边界
        else if (ewma_shortterm_activity_ < current_blockNum_perPage * shrink_threshold_) {
            shortterm_factor = std::max(ewma_shortterm_activity_, min_shortterm_activity_);
        }

        float res = longterm_factor == 0 ? shortterm_factor : longterm_factor;
        b_isRecycle_ = res < 1; // 如果res小于1，说明当前需求下降，允许缩页
        current_blockNum_perPage_ = res * current_blockNum_perPage;
        // 不能超过最小限制
        current_blockNum_perPage_ = std::max(static_cast<float>(min_blockNum_perPage_), current_blockNum_perPage_);
        // 不能超过最大上限
        current_blockNum_perPage_ = std::min(current_blockNum_perPage_, static_cast<float>(max_blockNum_perPage_));
    }
} // TinyRenderer