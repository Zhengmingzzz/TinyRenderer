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
    // �ڴ�ռ���ʵĵȼ�����
    // [0,20]Ϊ�ͼ���[20,70]Ϊ�м���[70,100]Ϊ�߼�
    enum class Occupancy_level {
        low = 0,
        low_medium,
        medium = 20,
        medium_medium,
        high = 70,
    };

    class MemoryManager {
    public:
        static inline constexpr int MAX_TWOPOWERI = 10;  // ����������2�Ķ��ٴη���С������[0, MAX_TWOPOWERI]��С���ڴ��
        static inline constexpr int MAX_BLOCKNUM_PERPAGE = 20; // ÿ��ҳ���20���� ����ܳ���0xff-1��������Ҫ����Page�߼�
        static inline int refresh_interval_ = 15; // ÿ������֡����һ��ָ��

        Occupancy_level occupancy_level_; // ԭ�ӵ�ռ���ʵȼ������ڿ���Allocator��ҳ�������
    public:
        void startUp();
        void shutDown();
        void* allocate(size_t size);
        void deallocate(void* ptr);
        void tick();  // ÿ֡tickһ�Σ��ж��Ƿ����ָ��
        void refreshMetrics(); // ˢ��ָ��

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
        Ewma ewma_occupancy_; // �ƶ�ƽ���ڴ�ռ����,ͨ��������ռ���ʵȼ�


        struct PageCompare {
            bool operator()(const Page* a, const Page* b) const {
                return a < b;
            }
        };
        std::set<Page*, PageCompare> pages_; // ��С����Ϊpage�ĵ�ַ�Ŷ�
        SpinLock registerPage_spinlock_;

        static inline std::once_flag instance_flag_;
        static inline std::once_flag startUp_flag_;
        static inline std::once_flag shutDown_flag_;
    };

    // �����ڴ���ڴ�صĽӿ�
    template<typename T, typename... Args>
    T* newElement(Args... args) {
        Counter_Add(mem_new_cnt);
        StopWatch_Start(mem_new_time);
        T* ptr = nullptr;
#ifdef _DEBUG
        //TODO:�ĳ�DEBUG_MemoryManager
        if ((ptr = reinterpret_cast<T*>(MemoryManager::instance().allocate(sizeof(T)))) != nullptr) {
            new(ptr) T(std::forward<Args>(args)...);
        }
#elifdef _RELEASE
        // ���ptr��Ϊ�������operator new��ʼ��
        if ((ptr = MemoryManager::instance().allocate(sizeof(T))) != nullptr) {
            new(ptr) T(std::forward<Args>(args)...);
        }
#endif
        StopWatch_Pause(mem_new_time);
        return ptr;
    }

    // �ͷ��ڴ���ڴ�صĽӿ�
    template<typename T>
    void deleteElement(T* ptr) {
        StopWatch_Start(mem_delete_time);
        if (ptr != nullptr) {
            ptr->~T();
#ifdef _DEBUG
            // TODO:��ΪDEBUG_MemoryManager
            MemoryManager::instance().deallocate(ptr);
#elifdef _RELEASE
            MemoryManager::instance().deallocate(ptr);
#endif
        StopWatch_Pause(mem_delete_time);
        }
    }
} // TinyRenderer

