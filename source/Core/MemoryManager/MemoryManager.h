//
// Created by Administrator on 25-4-15.
//
#pragma once
#include <memory>
#include "array"
#include "Allocator.h"
#include <set>

#include "Core/DebugMemoryManager/DebugMemoryManager.h"
#include "Function/Counter/CounterManager.h"
#include "Function/StopWatch/StopWatchManager.h"

#include <source_location>

namespace TinyRenderer {
    // 内存占用率的等级划分
    // [0,20]为低级，[20,70]为中级，[70,100]为高级
    enum class Occupancy_level {
        low = 0,
        low_medium,
        medium = 20,
        medium_medium,
        high = 70,
    };

    class MemoryManager {
        friend class DebugMemoryManager;
    public:
        static inline constexpr int MAX_TWOPOWERI = 10;  // 最大可以申请2的多少次方大小，共有[0, MAX_TWOPOWERI]大小的内存块
        static inline constexpr int MIN_BLOCKNUM_PERPAGE = 1; // 每页最小可以有多少个块
        static inline int refresh_interval_ = 15; // 每过多少帧更新一次指标

        Occupancy_level occupancy_level_; // 原子的占用率等级，用于控制Allocator的页分配策略
    public:
        void startUp(int min_blockNum_perPage = 1);
        void shutDown();
        void* allocate(size_t size);
        void deallocate(void* ptr);
        void tick();  // 每帧tick一次，判断是否更新指标
        void refreshMetrics(); // 刷新指标
        void register_newPage(Page* new_page); // 注册新页
    public:
        static MemoryManager& instance();
        MemoryManager(const MemoryManager&) = delete;
        MemoryManager& operator=(const MemoryManager&) = delete;


    private:
        struct Ewma {
            float smoothing_factor_;
            float average_occupancy_;
        };
        MemoryManager() = default;
        std::array<std::unique_ptr<Allocator>, MAX_TWOPOWERI+1> allocators_;
        Ewma ewma_occupancy_; // 移动平均内存占用率,通过它调整占用率等级


        struct PageCompare {
            bool operator()(const Page* a, const Page* b) const {
                return a < b;
            }
        };
        std::set<Page*, PageCompare> pages_; // 从小到大为page的地址排队

        static inline std::once_flag instance_flag_;
        static inline std::once_flag startUp_flag_;
        static inline std::once_flag shutDown_flag_;
    };

#ifdef _DEBUG
    #define newElement(T, ...) newElement_imp<T>(__FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define newElement(T, ...) newElement_imp<T>(__VA_ARGS__)
#endif

#ifdef _DEBUG
    // DEBUG模式下带参数的内存分配
    template<typename T, typename... Args>
    T* newElement_imp(const char* file, int line, Args&&... args) {
        T* ptr = nullptr;
        // 如果ptr不为空则调用operator new初始化
        if ((ptr = reinterpret_cast<T*>(MemoryManager::instance().allocate(sizeof(T)))) != nullptr) {
            new(ptr) T(std::forward<Args>(args)...);
        }
        if (ptr) {
            int block_size = 1;
            while (block_size<sizeof(T)) {
                block_size<<=1;
            }
            DEBUG_MEM_ALLOCATE_BLOCK(block_size, ptr, file, line);
        }
        return ptr;
    }
#endif
    // RELEASE模式下不带参数的内存分配`
    // 申请内存的内存池的接口
    template<typename T, typename... Args>
    T* newElement_imp(Args... args) {
        T* ptr = nullptr;
        // 如果ptr不为空则调用operator new初始化
        if ((ptr = reinterpret_cast<T*>(MemoryManager::instance().allocate(sizeof(T)))) != nullptr) {
            new(ptr) T(std::forward<Args>(args)...);
        }
        return ptr;
    }
    // 释放内存的内存池的接口
    template<typename T>
    void deleteElement(T* ptr) {
        if (ptr != nullptr) {
            ptr->~T();
            MemoryManager::instance().deallocate(ptr);
            DEBUG_MEM_DEALLOCATE_BLOCK(sizeof(T), ptr);
        }
    }
} // TinyRenderer

