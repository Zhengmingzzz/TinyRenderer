//
// Created by Administrator on 25-4-15.
//
#pragma once
#include <mutex>
#include "Page.h"

namespace TinyRenderer {
    enum class Occupancy_level : int; // ǰ������

    class Allocator {
    public:
        int block_size_; // ���Allocator���������Ŀ�
        int max_blockNum_perPage_; // �����Է������
        int min_blockNum_perPage_;
        float current_blockNum_perPage_; // ��ǰһ��ҳ���Է�����ٿ飬����������Сʱʹ��float���ͣ�����������תΪint����
        bool b_isRecycle_; // ������ҳ�Ƿ����
        Page* priority_pagehead_; // ˫��ѭ��������ÿ������Ԫ�ػ��Զ�����˳�򣻱�ʾ��ǰallocator���е�page�ĵ�һ��page
        Page* closed_pagelist_; // �����������ʾ����Ϊ0��ҳ

    // �������ָ���һЩ����
    public:
        // ����ƽ����Ծֵ��
        float ewma_longterm_activity_;
        float ewma_longterm_activity_factor_;
        // ��Ծֵ�����/��С���ƣ���ֹ��������ҳ�Ĵ�С
        float max_longterm_activity_;
        float min_longterm_activity_;

        // ����ƽ����Ծֵ��
        float ewma_shortterm_activity_;
        float ewma_shortterm_activity_factor_;
        float max_shortterm_activity_;
        float min_shortterm_activity_;

        // ҳ������/��С��ֵ
        float expand_threshold_;
        float shrink_threshold_;

        // ���㵱ǰ֡new/delete����
        size_t new_count_;
        size_t delete_count_;

    public:
        std::mutex allocator_mutex_;
        SpinLock allocator_spinlock_;


    public:
        void startUp(size_t block_size,size_t min_blockNum_perPage, size_t max_blockNum_perPage);
        void shutDown();

        void* allocate();
        bool try_dellocate(Page* target_page, Block* target_block, bool& isRecycle_page); // ��MemoryManager�ṩָ����page����ptr,Allocator�ж�ptr��Page��

        void tick();
        void refreshMetrics(Occupancy_level occupency_level);
    private:
        void insert_page(Page*& pagehead, Page* new_page);
        void insert_closedlist(Page*& pagehead, Page* target_page);
        void remove_page(Page*& pagehead, Page* target_page);
        void swap_adjacent_page(Page* page1, Page* page2); // �����������ڵ�ҳ��pagelist��
    };

} // TinyRenderer
