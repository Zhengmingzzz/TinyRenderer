//
// Created by Administrator on 25-4-15.
//
#pragma once
#include <memory>
#include "array"
#include "Allocator.h"
#include "Function/Message/Message.h"
#include <set>

#include "Function/Counter/CounterManager.h"
#include "Function/StopWatch/StopWatchManager.h"

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
    public:
        static inline constexpr int MAX_TWOPOWERI = 10;  // 最大可以申请2的多少次方大小，共有[0, MAX_TWOPOWERI]大小的内存块
        static inline constexpr int MAX_BLOCKNUM_PERPAGE = 20; // 每个页最多20个块 最大不能超过0xff-1，否则需要更改Page逻辑
        static inline int refresh_interval_ = 15; // 每过多少帧更新一次指标

        Occupancy_level occupancy_level_; // 原子的占用率等级，用于控制Allocator的页分配策略
    public:
        void startUp();
        void shutDown();
        void* allocate(size_t size);
        void deallocate(void* ptr);
        void tick();  // 每帧tick一次，判断是否更新指标
        void refreshMetrics(); // 刷新指标

        void register_newPage(Page* new_page);

    public:
        static MemoryManager& instance();
        MemoryManager(const MemoryManager&) = delete;
        MemoryManager& operator=(const MemoryManager&) = delete;


    private:
        struct Ewma {
            float smoothing_factor_;
            float average_occupancy_;
        };
        MemoryManager(){};
        std::array<std::unique_ptr<Allocator>, MAX_TWOPOWERI+1> allocators_;
        Ewma ewma_occupancy_; // 移动平均内存占用率,通过它调整占用率等级


        struct PageCompare {
            bool operator()(const Page* a, const Page* b) const {
                return a < b;
            }
        };
        std::set<Page*, PageCompare> pages_; // 从小到大为page的地址排队
        SpinLock registerPage_spinlock_;

        static inline std::once_flag instance_flag_;
        static inline std::once_flag startUp_flag_;
        static inline std::once_flag shutDown_flag_;
    };

    // 申请内存的内存池的接口
    template<typename T, typename... Args>
    T* newElement(Args... args) {
        Counter_Add(mem_new_cnt);
        StopWatch_Start(mem_new_time);
        T* ptr = nullptr;
#ifdef _DEBUG
        //TODO:改成DEBUG_MemoryManager
        if ((ptr = reinterpret_cast<T*>(MemoryManager::instance().allocate(sizeof(T)))) != nullptr) {
            new(ptr) T(std::forward<Args>(args)...);
        }
#elifdef _RELEASE
        // 如果ptr不为空则调用operator new初始化
        if ((ptr = MemoryManager::instance().allocate(sizeof(T))) != nullptr) {
            new(ptr) T(std::forward<Args>(args)...);
        }
#endif
        StopWatch_Pause(mem_new_time);
        return ptr;
    }

    // 释放内存的内存池的接口
    template<typename T>
    void deleteElement(T* ptr) {
        StopWatch_Start(mem_delete_time);
        if (ptr != nullptr) {
            ptr->~T();
#ifdef _DEBUG
            // TODO:改为DEBUG_MemoryManager
            MemoryManager::instance().deallocate(ptr);
#elifdef _RELEASE
            MemoryManager::instance().deallocate(ptr);
#endif
        StopWatch_Pause(mem_delete_time);
        }
    }
} // TinyRenderer

