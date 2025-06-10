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
#include <rttr/registration>
#include "Function/Framework/HierarchyNode/HierarchyNode.h"

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
        static inline constexpr int MAX_TWOPOWERI = 20;  // 最大可以申请2的多少次方大小，共有[0, MAX_TWOPOWERI]大小的内存块
        static inline constexpr int MIN_BLOCKNUM_PERPAGE = 1; // 每页最小可以有多少个块
        // static inline int refresh_interval_ = 15; // 每过多少帧更新一次指标

        Occupancy_level occupancy_level_; // 原子的占用率等级，用于控制Allocator的页分配策略
    public:
        void startUp();
        void shutdown();
        void* allocate(size_t size);
        void deallocate(void* ptr);
        // void tick();  // 每帧tick一次，判断是否更新指标
        // void refreshMetrics(); // 刷新指标
        void register_newPage(Page* new_page); // 注册新页
    public:
        static MemoryManager& get_instance();
        MemoryManager(const MemoryManager&) = delete;
        MemoryManager& operator=(const MemoryManager&) = delete;


    private:
        // struct Ewma {
        //     float smoothing_factor_;
        //     float average_occupancy_;
        // };
        MemoryManager() = default;
        std::array<std::unique_ptr<Allocator>, MAX_TWOPOWERI+1> allocators_;
        // Ewma ewma_occupancy_; // 移动平均内存占用率,通过它调整占用率等级


        struct PageCompare {
            bool operator()(const Page* a, const Page* b) const {
                return a < b;
            }
        };
        std::set<Page*, PageCompare> pages_; // 从小到大为page的地址排队

        static inline std::once_flag instance_flag_;
        static inline std::once_flag startUp_flag_; // TODO:修改startUp_flag_和shutDown_flag_
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
        if ((ptr = reinterpret_cast<T*>(MemoryManager::get_instance().allocate(sizeof(T)))) != nullptr) {
            new(ptr) T(std::forward<Args>(args)...);
        }
        if (ptr) {
            int twopoweri = 0;
            int t_num = 1;
            // 找到大于size的第一个allocator
            for (int i=0;i<=MemoryManager::MAX_TWOPOWERI;i++) {
                twopoweri = i;
                if (t_num >= sizeof(T))
                    break;
                t_num <<= 1;
            }
            DEBUG_MEM_ALLOCATE_BLOCK(twopoweri, ptr, file, line);
        }
        return ptr;
    }
#else
#ifdef _RELEASE
    // RELEASE模式下不带参数的内存分配`
    // 申请内存的内存池的接口
    template<typename T, typename... Args>
    T* newElement_imp(Args... args) {
        T* ptr = nullptr;
        // 如果ptr不为空则调用operator new初始化
        if ((ptr = reinterpret_cast<T*>(MemoryManager::get_instance().allocate(sizeof(T)))) != nullptr) {
            new(ptr) T(std::forward<Args>(args)...);
        }
        return ptr;
    }
#endif
#endif
    // 释放内存的内存池的接口
    template<typename T>
    void deleteElement(T* ptr) {
        if (ptr != nullptr) {
            // uintptr_t tmp_address_uint = (uintptr_t)ptr;
            // tmp_address_uint -= sizeof(void*);
            // // if (rttr::type::get<T>().is_derived_from(rttr::type::get<HierarchyNode>())) {
            // //     tmp_address_uint -= sizeof(HierarchyNode);
            // // }
            // void* tmp_address = reinterpret_cast<void*>(tmp_address_uint);


            ptr->~T();
            MemoryManager::get_instance().deallocate(ptr);
        }
    }
} // TinyRenderer

