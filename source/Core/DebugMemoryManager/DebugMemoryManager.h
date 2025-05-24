//
// Created by Administrator on 25-4-23.
//
#pragma once
#include "DebugAllocator.h"
#include "Function/CommonType/json.h"


namespace TinyRenderer {
    class MemoryManager;
    class DebugMemoryManager {
    public:
        void startup();
        void shutDown();

        // void on_allocator_startUp(int block_size, int min_block_num_perPage, int max_block_num_perPage);

        void on_alloc_block(int block_size,void* block, std::string&& file, size_t line);
        void on_dealloc_block(int block_size, void* block);

        // void on_allocate_newPage(void* page, int block_size, int block_num);


    private:
        //ordered_json log_mem_info();
    private:
        // using time_point = std::chrono::system_clock::time_point;
        //
        // size_t total_alloc_cnt_;
        std::vector<DebugAllocator> debug_allocators_;
        // time_point start_time_;
        // time_point end_time_;
        // size_t survival_time_;
    public:
        static DebugMemoryManager& instance();
        DebugMemoryManager(const DebugMemoryManager&) = delete;
        DebugMemoryManager& operator=(const DebugMemoryManager&) = delete;

    private:
        DebugMemoryManager() = default;
    };

} // TinyRenderer

#ifdef _DEBUG
// 生命周期管理宏组
    #define DEBUG_MEM_STARTUP() DebugMemoryManager::instance().startup();
    #define DEBUG_MEM_SHUTDOWN() DebugMemoryManager::instance().shutDown();

// 内存分配追踪宏组
    #define DEBUG_MEM_ALLOCATE_BLOCK(size, block_addr, file, line) DebugMemoryManager::instance().on_alloc_block(size, block_addr, std::move(file), line);
    #define DEBUG_MEM_DEALLOCATE_BLOCK(size,ptr) DebugMemoryManager::instance().on_dealloc_block(size, ptr);

    //#define DEBUG_MEM_ALLOCATE_NEW_PAGE(size, page_ptr, block_num) DebugMemoryManager::instance().on_allocate_newPage(page_ptr, size, block_num);
    //#define DEBUG_MEM_DEALLOCATE_NEW_PAGE(size, page_ptr) DebugMemoryManager::instance().on_deallocate_page(size, page_ptr);

    //#define DEBUG_MEM_ALLOCATOR_STARTUP(size, min_block_num, max_block_num) DebugMemoryManager::instance().on_allocator_startUp(size, min_block_num, max_block_num);

    // #define DEBUG_MEM_ADJUST_METRIC(block_size, current_occupancy, longterm_factor, \
    //     longterm_threshold, shortterm_factor, shortterm_threshold, current_blockNum_perPage) \
    //     DebugMemoryManager::instance().on_adjust_metric(block_size, current_occupancy, \
    //     longterm_factor, longterm_threshold, shortterm_factor, shortterm_threshold, current_blockNum_perPage);
#else
    // Release模式空定义
    #define DEBUG_MEM_STARTUP(...)              // 无参数宏直接空定义
    #define DEBUG_MEM_SHUTDOWN(...)

    #define DEBUG_MEM_ALLOCATE_BLOCK(...)  // 带参宏保留参数列表
    #define DEBUG_MEM_DEALLOCATE_BLOCK(...)

    #define DEBUG_MEM_ALLOCATE_NEW_PAGE(...)
    #define DEBUG_MEM_DEALLOCATE_NEW_PAGE(...)

    #define DEBUG_MEM_ALLOCATOR_STARTUP(...)

    #define DEBUG_MEM_ADJUST_METRIC(...)
#endif