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
            // 1. �ж��Ƿ��пյĿ�
            if (cur_page == nullptr) {
                // ˫�ؼ������������ҳ
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

            // TODO:�˴����ܻ��о�̬��������ɾ��������һ��page�󣬿��ܻ��ȡ��������
            // 2. ���京�п�����ҳ��û�����������һ��ҳ
            Counter_Add(mem_try_allocate_block_cnt);
            // TODO:����С���Ѵ��Page���ٱ���������ʹ�������ſ���Ϊ���Page��
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

        bool is_free = true; // �Ƿ��ptrʹ��free
        if (target_page->try_deallocate_block(target_block)) {
            delete_count_ += 1;
            is_free = false;

            // ���ҳΪ���ˣ�����b_isRecycle�ж��Ƿ�Ҫ����ҳ
                if (target_page->current_block_num_ == target_page->total_block_num_) {
                    // ��ʱ�����ջ�ҳ
                    if (b_isRecycle_) {
                        // ���Ҫɾ��ͷ��㣬�����ͷ���Ϊ��ǰ�ڵ����һ���ڵ�
                        if (target_page == pagehead_) {
                            pagehead_ = target_page->next; // ����ͷ�ڵ�����
                            if (target_page == pagetail_) {
                                pagetail_ = nullptr;    // �������ͷ�������β�������
                            }
                        }
                        // �����м�ڵ�����
                        else {
                            Page* prev = pagehead_;
                            while (prev != nullptr && prev->next != target_page) {
                                prev = prev->next;
                            }
                            ASSERT(prev!=nullptr);
                            if (prev) { // �ҵ�ǰ���ڵ�
                                prev->next = target_page->next;
                                if (pagetail_ == target_page) { // ����βָ��
                                    pagetail_ = prev;
                                }
                            }
                        }


                        // �ͷ����ҳ
                        free(target_page);
                        // ֪ͨMemoryManager�Ѿ����ҳ���ͷ�
                        isRecycle_page = true;
                    }
                }

        }
        // ���ؿ��Ƿ��ջ�
        return !is_free;
    }

    void Allocator::tick() {
        float difference = new_count_ - delete_count_;
        // �����ǰû��new��û��delete��Ĭ�ϼ��ٲ���ҳ
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
            // ���/��С��������Ϊԭ��ҳ�Ķ��ٱ�
            max_longterm_activity_ = 2.f;
            min_longterm_activity_ = 0.7f;
            max_shortterm_activity_ = 1.5f;
            min_shortterm_activity_ = 0.8f;
            // ҳ�ﵽ����/������������ֵ
            expand_threshold_ = 0.8f;
            shrink_threshold_ = 0.6f;
        }
        else if (occupancy_level == Occupancy_level::medium) {
            // ���/��С��������Ϊԭ��ҳ�Ķ��ٱ�
            max_longterm_activity_ = 1.5f;
            min_longterm_activity_ = 0.7f;
            max_shortterm_activity_ = 1.1f;
            min_shortterm_activity_ = 0.9f;
            // ҳ�ﵽ����/������������ֵ
            expand_threshold_ = 1.2f;
            shrink_threshold_ = 0.4f;
        }
        else if (occupancy_level == Occupancy_level::high) {
            // ���/��С��������Ϊԭ��ҳ�Ķ��ٱ�
            max_longterm_activity_ = 1.2f;
            min_longterm_activity_ = 0.3f;
            max_shortterm_activity_ = 1.f;
            min_shortterm_activity_ = 0.8f;
            // ҳ�ﵽ����/������������ֵ
            expand_threshold_ = 1.5;
            shrink_threshold_ = 0.2f;
        }

        float longterm_factor = 0, shortterm_factor = 0;
        // ҳ�ķ����С�Գ��ڻ�ԾֵΪ�������ڻ�Ծֵ���ڸ��ݵ�ǰ���΢��
        float current_blockNum_perPage = static_cast<float>(current_blockNum_perPage_);
        // ��ǰ���ڻ�Ծֵ�������ŵı߽�
        if (ewma_longterm_activity_ > current_blockNum_perPage * expand_threshold_) {
            // ����ÿҳ�����������������������
            longterm_factor = std::min(ewma_longterm_activity_, max_longterm_activity_);
        }
        // ��ǰ���ڻ�ԾֵС����С�ı߽�
        else if (ewma_longterm_activity_ < current_blockNum_perPage * shrink_threshold_) {
            // ��Сÿҳ������������������С����
            longterm_factor = std::max(ewma_longterm_activity_, min_longterm_activity_);
        }
        // ��ǰ���ڻ�Ծֵ�������ŵı߽�
        if (ewma_shortterm_activity_ > current_blockNum_perPage * expand_threshold_) {
            shortterm_factor = std::min(ewma_shortterm_activity_, max_shortterm_activity_);
        }
        // ��ǰ���ڻ�ԾֵС����С�ı߽�
        else if (ewma_shortterm_activity_ < current_blockNum_perPage * shrink_threshold_) {
            shortterm_factor = std::max(ewma_shortterm_activity_, min_shortterm_activity_);
        }

        float res = longterm_factor == 0 ? shortterm_factor : longterm_factor;
        b_isRecycle_ = res < 1; // ���resС��1��˵����ǰ�����½���������ҳ
        current_blockNum_perPage_ = res * current_blockNum_perPage;
        // ���ܳ�����С����
        current_blockNum_perPage_ = std::max(static_cast<float>(min_blockNum_perPage_), current_blockNum_perPage_);
        // ���ܳ����������
        current_blockNum_perPage_ = std::min(current_blockNum_perPage_, static_cast<float>(max_blockNum_perPage_));
    }
} // TinyRenderer