//
// Created by Administrator on 25-4-15.
//

#include "Allocator.h"

#include "Page.h"
#include "Function/Message/Message.h"
#include "MemoryManager.h"

#include "Core/DebugMemoryManager/DebugAllocator.h"

namespace TinyRenderer {
    void Allocator::startUp(size_t block_size,size_t blockNum_perPage) {
        ASSERT(block_size>0);
        ASSERT(blockNum_perPage>0);

        block_size_ = block_size;
        blockNum_perPage_ = blockNum_perPage;
        priority_pagehead_ = nullptr;
        closed_pagelist_ = nullptr;

        // DEBUG_MEM_ALLOCATOR_STARTUP(block_size, min_blockNum_perPage, max_blockNum_perPage);
    }

    void Allocator::shutdown() {
        Page* cur_page = priority_pagehead_, *next_page = nullptr;
        // 处理pagelist
        // 把最后一个节点的next指向nullptr，防止循环遍历
        if (cur_page != nullptr)
            cur_page->prev->next = nullptr;
        while (cur_page != nullptr) {
            cur_page->shutdown();
            next_page = cur_page->next;
            free(cur_page);
            cur_page = next_page;
        }

        // 处理closed_pagelist
        cur_page = closed_pagelist_;
        if (cur_page != nullptr)
            cur_page->prev->next = nullptr;
        while (cur_page != nullptr) {
            cur_page->shutdown();
            next_page = cur_page->next;
            free(cur_page);
            cur_page = next_page;
        }
    }

    void* Allocator::allocate() {
        //std::lock_guard mutex_lock(allocator_mutex_);
        void* res = nullptr;
        Page* cur_page = priority_pagehead_;

        // 1. 判断是否还有空的块
        // priority_pagehead_为空说明没有空余块的页了
        if (cur_page == nullptr) {
            Page* new_page = Page::allocate_newPage(block_size_, blockNum_perPage_, this);
            insert_page(priority_pagehead_, new_page); // 插入新页到优先链表
            cur_page = new_page;
        }

        // 2. 分配含有空余块的页
        // cur_page必定会有空余的块
        if (cur_page->allocate_block(res)) {
            // 如果当前页块数耗尽，将这个页转移出优先链表，移入closed_pagelist
            if (cur_page->current_block_num_ == 0) {
                remove_page(priority_pagehead_ ,cur_page); // 将cur_page移出pagelist，插入closed_pagelist
                insert_page(closed_pagelist_, cur_page);

            }
        }

        return res;
    }

    bool Allocator::try_dellocate(Page* const target_page, Block* const target_block, bool& isRecycle_page) {
        ASSERT(target_page!=nullptr);
        ASSERT(target_block!=nullptr);

        //std::lock_guard lock(allocator_mutex_);

        if (target_page->try_deallocate_block(target_block) == false)
            return false;

        // delete_count_ += 1;
        DEBUG_MEM_DEALLOCATE_BLOCK(block_size_, target_block);

        // 如果页为空了，根据b_isRecycle判断是否要回收页
        if (target_page->current_block_num_ == target_page->total_block_num_) {
            recycle_time_--;
            if (recycle_time_ <= 0)
                b_isRecycle_ = true;
            // 此时可以收回页
            if (b_isRecycle_) {
                recycle_time_ = 3; // 恢复回收判断
                b_isRecycle_ = false;

                // 从priority_pagelist取出该节点
                Page* prev_page = target_page->prev;
                Page* next_page = target_page->next;
                prev_page->next = next_page;
                next_page->prev = prev_page;
                // 如果更新的页节点为头结点
                if (target_page == priority_pagehead_) {
                    // 如果只有一个节点，则置空；否则更新为next节点
                    priority_pagehead_ = (next_page == priority_pagehead_) ? nullptr : next_page;
                }
                // 释放这个页
                free(target_page);
                // 通知MemoryManager已经完成页的释放，删除对应节点
                isRecycle_page = true;
                //DEBUG_MEM_DEALLOCATE_NEW_PAGE(block_size_, target_page);
            }
        }
        // 调整pagelist的位置
        else {
            // 代表这个页之前块数为0，现在释放了一个块变为1，存储于closed_pagelist中
            if (target_page->current_block_num_ == 1) {
                // 从closed_list中拿出来，转到pagelist中
                remove_page(closed_pagelist_, target_page);
                insert_page(priority_pagehead_, target_page);
            }
            // 反之则说明这个page是在pagelist中，对它进行位置的调整
            else {
                Page* prev_page = target_page->prev;
                Page* next_page = target_page->next;
                // 当前更改的页数量大于下一个页，向后移
                if (target_page->current_block_num_ > next_page->current_block_num_) {
                    swap_adjacent_page(target_page, next_page);
                    if (target_page == priority_pagehead_) {
                        priority_pagehead_ = next_page;
                    }
                }
                // 当前更改的页数量小于上一个页，向前移
                else if (target_page->current_block_num_ < prev_page->current_block_num_) {
                    swap_adjacent_page(prev_page, target_page);
                    if (target_page == priority_pagehead_) {
                        priority_pagehead_ = prev_page;
                    }
                }
            }
        }

        // 返回块是否被收回
        return true;
    }

    // void Allocator::tick() {
    //     float difference = new_count_ - delete_count_;
    //     // 如果当前没有new，没有delete，默认减少部分页
    //     if (new_count_ == 0 && delete_count_ == 0) {
    //         difference = 0.95f;
    //     }
    //     ewma_longterm_activity_ = (ewma_longterm_activity_ * ewma_longterm_activity_factor_) + difference * (1 - ewma_longterm_activity_factor_);
    //     ewma_shortterm_activity_ = (ewma_shortterm_activity_ * ewma_shortterm_activity_factor_) + difference * (1 - ewma_shortterm_activity_factor_);
    //     new_count_ = 0;
    //     delete_count_ = 0;
    // }

    // void Allocator::refreshMetrics(Occupancy_level occupancy_level) {
    //     if (occupancy_level == Occupancy_level::low) {
    //         // 最大/最小可以缩放为原来页的多少倍
    //         max_longterm_activity_ = 2.f;
    //         min_longterm_activity_ = 0.7f;
    //         max_shortterm_activity_ = 1.5f;
    //         min_shortterm_activity_ = 0.8f;
    //         // 页达到扩张/缩放条件的阈值
    //         expand_threshold_ = 0.8f;
    //         shrink_threshold_ = 0.6f;
    //     }
    //     else if (occupancy_level == Occupancy_level::medium) {
    //         // 最大/最小可以缩放为原来页的多少倍
    //         max_longterm_activity_ = 1.5f;
    //         min_longterm_activity_ = 0.7f;
    //         max_shortterm_activity_ = 1.1f;
    //         min_shortterm_activity_ = 0.9f;
    //         // 页达到扩张/缩放条件的阈值
    //         expand_threshold_ = 1.2f;
    //         shrink_threshold_ = 0.4f;
    //     }
    //     else if (occupancy_level == Occupancy_level::high) {
    //         // 最大/最小可以缩放为原来页的多少倍
    //         max_longterm_activity_ = 1.2f;
    //         min_longterm_activity_ = 0.3f;
    //         max_shortterm_activity_ = 1.f;
    //         min_shortterm_activity_ = 0.8f;
    //         // 页达到扩张/缩放条件的阈值
    //         expand_threshold_ = 1.5;
    //         shrink_threshold_ = 0.2f;
    //     }
    //
    //     float longterm_factor = 0, shortterm_factor = 0;
    //     // 页的分配大小以长期活跃值为主，短期活跃值用于根据当前情况微调
    //     float current_blockNum_perPage = current_blockNum_perPage_;
    //     // 当前长期活跃值大于扩张的边界
    //     if (ewma_longterm_activity_ > current_blockNum_perPage * expand_threshold_) {
    //         // 增加每页数量，但是限制其最大上限
    //         longterm_factor = std::min(ewma_longterm_activity_, max_longterm_activity_);
    //     }
    //     // 当前长期活跃值小于缩小的边界
    //     else if (ewma_longterm_activity_ < current_blockNum_perPage * shrink_threshold_) {
    //         // 缩小每页数量，但是限制其最小下限
    //         longterm_factor = std::max(ewma_longterm_activity_, min_longterm_activity_);
    //     }
    //     // 当前短期活跃值大于扩张的边界
    //     if (ewma_shortterm_activity_ > current_blockNum_perPage * expand_threshold_) {
    //         shortterm_factor = std::min(ewma_shortterm_activity_, max_shortterm_activity_);
    //     }
    //     // 当前短期活跃值小于缩小的边界
    //     else if (ewma_shortterm_activity_ < current_blockNum_perPage * shrink_threshold_) {
    //         shortterm_factor = std::max(ewma_shortterm_activity_, min_shortterm_activity_);
    //     }
    //
    //     float res = longterm_factor == 0 ? shortterm_factor : longterm_factor;
    //     b_isRecycle_ = res < 1; // 如果res小于1，说明当前需求下降，允许缩页
    //     current_blockNum_perPage_ = res * current_blockNum_perPage;
    //     // 不能超过最小限制
    //     current_blockNum_perPage_ = std::max(static_cast<float>(min_blockNum_perPage_), current_blockNum_perPage_);
    //     // 不能超过最大上限
    //     current_blockNum_perPage_ = std::min(current_blockNum_perPage_, static_cast<float>(max_blockNum_perPage_));
    //
    //     // DEBUG_MEM_ADJUST_METRIC(block_size_ ,static_cast<int>(occupancy_level), ewma_longterm_activity_, current_blockNum_perPage * expand_threshold_, ewma_shortterm_activity_, current_blockNum_perPage * shrink_threshold_, current_blockNum_perPage);
    // }

    void Allocator::insert_page(Page*& pagehead, Page* const new_page) {
        // 没有节点的情况
        if (pagehead == nullptr) {
            pagehead = new_page;
            new_page->next = pagehead;
            new_page->prev = pagehead;
            return;
        }
        // 当前首页的块数大于新页的块数，则插入到头结点
        if (pagehead->current_block_num_ >= new_page->current_block_num_) {
            new_page->next = pagehead;
            new_page->prev = pagehead->prev;
            pagehead->prev->next = new_page;
            pagehead->prev = new_page;
            return;
        }

        //大于等于1个节点的情况
        // 将new_page插入到cur_page之后
        Page* cur_page = pagehead;
        //找到第一个小于new_page块数的节点
        while (cur_page->prev != pagehead && cur_page->prev->current_block_num_ > new_page->current_block_num_) {
            cur_page = cur_page->prev;
        }

        new_page->next = cur_page->next;
        new_page->prev = cur_page;
        cur_page->prev->next = new_page;
        cur_page->prev = new_page;
    }

    void Allocator::remove_page(Page*& pagehead, Page* const target_page) {
        ASSERT(target_page != nullptr);

        // 从priority_pagelist取出该节点
        Page* prev_page = target_page->prev;
        Page* next_page = target_page->next;

        prev_page->next = next_page;
        next_page->prev = prev_page;

        // 如果更新的页节点为头结点
        if (target_page == pagehead) {
            // 如果只有一个节点，则置空；否则更新为next节点
            pagehead = (next_page == pagehead) ? nullptr : next_page;
        }
    }

    void Allocator::swap_adjacent_page(Page* const page_front, Page* const page_back) {
        ASSERT(page_front != nullptr && page_back != nullptr);
        ASSERT(page_front->next == page_back);
        ASSERT(page_back->prev == page_front);

        Page* prev_page = page_front->prev;
        Page* next_page = page_back->next;

        // 先把两个节点相连
        page_back->next = page_front;
        page_front->prev = page_back;

        page_back->prev = prev_page;
        page_front->next = next_page;

        prev_page->next = page_back;
        next_page->prev = page_front;
    }
} // TinyRenderer