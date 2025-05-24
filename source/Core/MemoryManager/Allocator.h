//
// Created by Administrator on 25-4-15.
//
#pragma once
#include <mutex>
#include "Page.h"

namespace TinyRenderer {
    enum class Occupancy_level : int; // 前向声明

    class Allocator {
    public:
        int block_size_; // 这个Allocator负责分配多大的块
        // int max_blockNum_perPage_; // 最大可以分配多少
        // int min_blockNum_perPage_;
        float blockNum_perPage_; // 当前一个页可以分配多少块，计算数量大小时使用float类型，分配数量是转为int类型
        bool b_isRecycle_; // 遇到空页是否回收
        int recycle_time_ = 3; // 连续3次page为空则回收
        Page* priority_pagehead_; // 双向循环链表，且每次增减元素会自动调整顺序；表示当前allocator所有的page的第一个page
        Page* closed_pagelist_; // 单向的链表，表示块数为0的页

    // 计算分配指标的一些参数
    public:
        // 长期平均活跃值：
        // float ewma_longterm_activity_;
        // float ewma_longterm_activity_factor_;
        // // 活跃值的最大/最小限制，防止过度缩放页的大小
        // float max_longterm_activity_;
        // float min_longterm_activity_;
        //
        // // 短期平均活跃值：
        // float ewma_shortterm_activity_;
        // float ewma_shortterm_activity_factor_;
        // float max_shortterm_activity_;
        // float min_shortterm_activity_;
        //
        // // 页的扩张/缩小阈值
        // float expand_threshold_;
        // float shrink_threshold_;

        // 计算当前帧new/delete次数
        // size_t new_count_;
        // size_t delete_count_;

    public:
        std::mutex allocator_mutex_;
        SpinLock allocator_spinlock_;


    public:
        void startUp(size_t block_size,size_t blockNum_perPage);
        void shutDown();

        void* allocate();
        bool try_dellocate(Page* target_page, Block* target_block, bool& isRecycle_page); // 由MemoryManager提供指定的page回收ptr,Allocator判断ptr在Page中

        void tick();
        // void refreshMetrics(Occupancy_level occupency_level);
    private:
        void insert_page(Page*& pagehead, Page* new_page);
        void insert_closedlist(Page*& pagehead, Page* target_page);
        void remove_page(Page*& pagehead, Page* target_page);
        void swap_adjacent_page(Page* page1, Page* page2); // 交换两个相邻的页在pagelist中
    };

} // TinyRenderer
