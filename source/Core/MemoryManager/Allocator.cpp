//
// Created by Administrator on 25-4-15.
//

#include "Allocator.h"

#include <bits/fs_fwd.h>

#include "Page.h"
#include "Function/Message/Message.h"
#include "MemoryManager.h"
#include "Function/Counter/CounterManager.h"

#include "Core/DebugMemoryManager/DebugAllocator.h"

namespace TinyRenderer {
    void Allocator::startUp(size_t block_size,size_t min_blockNum_perPage, size_t max_blockNum_perPage) {
        ASSERT(block_size>0);
        ASSERT(max_blockNum_perPage>0);
        if (max_blockNum_perPage < min_blockNum_perPage) {
            min_blockNum_perPage = max_blockNum_perPage;
        }

        block_size_ = block_size;
        max_blockNum_perPage_ = max_blockNum_perPage;
        current_blockNum_perPage_ = min_blockNum_perPage;
        min_blockNum_perPage_ = min_blockNum_perPage;
        priority_pagehead_ = nullptr;
        closed_pagelist_ = nullptr;

        ewma_longterm_activity_ = 0.f;
        ewma_longterm_activity_factor_ = 0.7f;

        ewma_shortterm_activity_ = 0.f;
        ewma_shortterm_activity_factor_ = 0.3f;

        DEBUG_MEM_ALLOCATOR_STARTUP(block_size, min_blockNum_perPage, max_blockNum_perPage);
    }

    void Allocator::shutDown() {
        Page* cur_page = priority_pagehead_, *next_page = nullptr;
        // ����pagelist
        // �����һ���ڵ��nextָ��nullptr����ֹѭ������
        if (cur_page != nullptr)
            cur_page->prev->next = nullptr;
        while (cur_page != nullptr) {
            cur_page->shutDown();
            next_page = cur_page->next;
            free(cur_page);
            cur_page = next_page;
        }

        // ����closed_pagelist
        cur_page = closed_pagelist_;
        if (cur_page != nullptr)
            cur_page->prev->next = nullptr;
        while (cur_page != nullptr) {
            cur_page->shutDown();
            next_page = cur_page->next;
            free(cur_page);
            cur_page = next_page;
        }
    }

    void* Allocator::allocate() {
        //std::lock_guard mutex_lock(allocator_mutex_);
        void* res = nullptr;
        Page* cur_page = priority_pagehead_;

        // 1. �ж��Ƿ��пյĿ�
        // priority_pagehead_Ϊ��˵��û�п�����ҳ��
        if (cur_page == nullptr) {
            Page* new_page = Page::allocate_newPage(block_size_, static_cast<int>(current_blockNum_perPage_ + 0.9), this);
            insert_page(priority_pagehead_, new_page); // ������ҳ����������
            cur_page = new_page;
        }

        // 2. ���京�п�����ҳ
        // cur_page�ض����п���Ŀ�
        if (cur_page->allocate_block(res)) {
            // �����ǰҳ�����ľ��������ҳת�Ƴ�������������closed_pagelist
            if (cur_page->current_block_num_ == 0) {
                remove_page(priority_pagehead_ ,cur_page); // ��cur_page�Ƴ�pagelist������closed_pagelist
                insert_page(closed_pagelist_, cur_page);

            }
            new_count_ += 1;
        }

        return res;
    }

    bool Allocator::try_dellocate(Page* const target_page, Block* const target_block, bool& isRecycle_page) {
        ASSERT(target_page!=nullptr);
        ASSERT(target_block!=nullptr);

        //std::lock_guard lock(allocator_mutex_);

        if (target_page->try_deallocate_block(target_block) == false)
            return false;

        delete_count_ += 1;

        // ���ҳΪ���ˣ�����b_isRecycle�ж��Ƿ�Ҫ����ҳ
        if (target_page->current_block_num_ == target_page->total_block_num_) {
            // ��ʱ�����ջ�ҳ
            if (b_isRecycle_) {
                // ��priority_pagelistȡ���ýڵ�
                Page* prev_page = target_page->prev;
                Page* next_page = target_page->next;
                prev_page->next = next_page;
                next_page->prev = prev_page;
                // ������µ�ҳ�ڵ�Ϊͷ���
                if (target_page == priority_pagehead_) {
                    // ���ֻ��һ���ڵ㣬���ÿգ��������Ϊnext�ڵ�
                    priority_pagehead_ = (next_page == priority_pagehead_) ? nullptr : next_page;
                }
                // �ͷ����ҳ
                free(target_page);
                // ֪ͨMemoryManager�Ѿ����ҳ���ͷţ�ɾ����Ӧ�ڵ�
                isRecycle_page = true;
                //DEBUG_MEM_DEALLOCATE_NEW_PAGE(block_size_, target_page);
            }
        }
        // ����pagelist��λ��
        else {
            // �������ҳ֮ǰ����Ϊ0�������ͷ���һ�����Ϊ1���洢��closed_pagelist��
            if (target_page->current_block_num_ == 1) {
                // ��closed_list���ó�����ת��pagelist��
                remove_page(closed_pagelist_, target_page);
                insert_page(priority_pagehead_, target_page);
            }
            // ��֮��˵�����page����pagelist�У���������λ�õĵ���
            else {
                Page* prev_page = target_page->prev;
                Page* next_page = target_page->next;
                // ��ǰ���ĵ�ҳ����������һ��ҳ�������
                if (target_page->current_block_num_ > next_page->current_block_num_) {
                    swap_adjacent_page(target_page, next_page);
                    if (target_page == priority_pagehead_) {
                        priority_pagehead_ = next_page;
                    }
                }
                // ��ǰ���ĵ�ҳ����С����һ��ҳ����ǰ��
                else if (target_page->current_block_num_ < prev_page->current_block_num_) {
                    swap_adjacent_page(prev_page, target_page);
                    if (target_page == priority_pagehead_) {
                        priority_pagehead_ = prev_page;
                    }
                }
            }
        }

        // ���ؿ��Ƿ��ջ�
        return true;
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
        float current_blockNum_perPage = current_blockNum_perPage_;
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

        // DEBUG_MEM_ADJUST_METRIC(block_size_ ,static_cast<int>(occupancy_level), ewma_longterm_activity_, current_blockNum_perPage * expand_threshold_, ewma_shortterm_activity_, current_blockNum_perPage * shrink_threshold_, current_blockNum_perPage);
    }

    void Allocator::insert_page(Page*& pagehead, Page* const new_page) {
        // û�нڵ�����
        if (pagehead == nullptr) {
            pagehead = new_page;
            new_page->next = pagehead;
            new_page->prev = pagehead;
            return;
        }
        // ��ǰ��ҳ�Ŀ���������ҳ�Ŀ���������뵽ͷ���
        if (pagehead->current_block_num_ >= new_page->current_block_num_) {
            new_page->next = pagehead;
            new_page->prev = pagehead->prev;
            pagehead->prev->next = new_page;
            pagehead->prev = new_page;
            return;
        }

        //���ڵ���1���ڵ�����
        // ��new_page���뵽cur_page֮��
        Page* cur_page = pagehead;
        //�ҵ���һ��С��new_page�����Ľڵ�
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

        // ��priority_pagelistȡ���ýڵ�
        Page* prev_page = target_page->prev;
        Page* next_page = target_page->next;

        prev_page->next = next_page;
        next_page->prev = prev_page;

        // ������µ�ҳ�ڵ�Ϊͷ���
        if (target_page == pagehead) {
            // ���ֻ��һ���ڵ㣬���ÿգ��������Ϊnext�ڵ�
            pagehead = (next_page == pagehead) ? nullptr : next_page;
        }
    }

    void Allocator::swap_adjacent_page(Page* const page_front, Page* const page_back) {
        ASSERT(page_front != nullptr && page_back != nullptr);
        ASSERT(page_front->next == page_back);
        ASSERT(page_back->prev == page_front);

        Page* prev_page = page_front->prev;
        Page* next_page = page_back->next;

        // �Ȱ������ڵ�����
        page_back->next = page_front;
        page_front->prev = page_back;

        page_back->prev = prev_page;
        page_front->next = next_page;

        prev_page->next = page_back;
        next_page->prev = page_front;
    }
} // TinyRenderer