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
        friend class DebugMemoryManager;
    public:
        static inline constexpr int MAX_TWOPOWERI = 10;  // ����������2�Ķ��ٴη���С������[0, MAX_TWOPOWERI]��С���ڴ��
        static inline constexpr int MIN_BLOCKNUM_PERPAGE = 1; // ÿҳ��С�����ж��ٸ���
        static inline int refresh_interval_ = 15; // ÿ������֡����һ��ָ��

        Occupancy_level occupancy_level_; // ԭ�ӵ�ռ���ʵȼ������ڿ���Allocator��ҳ�������
    public:
        void startUp(int min_blockNum_perPage = 1);
        void shutDown();
        void* allocate(size_t size);
        void deallocate(void* ptr);
        void tick();  // ÿ֡tickһ�Σ��ж��Ƿ����ָ��
        void refreshMetrics(); // ˢ��ָ��
        void register_newPage(Page* new_page); // ע����ҳ
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
        Ewma ewma_occupancy_; // �ƶ�ƽ���ڴ�ռ����,ͨ��������ռ���ʵȼ�


        struct PageCompare {
            bool operator()(const Page* a, const Page* b) const {
                return a < b;
            }
        };
        std::set<Page*, PageCompare> pages_; // ��С����Ϊpage�ĵ�ַ�Ŷ�

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
    // DEBUGģʽ�´��������ڴ����
    template<typename T, typename... Args>
    T* newElement_imp(const char* file, int line, Args&&... args) {
        T* ptr = nullptr;
        // ���ptr��Ϊ�������operator new��ʼ��
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
    // RELEASEģʽ�²����������ڴ����`
    // �����ڴ���ڴ�صĽӿ�
    template<typename T, typename... Args>
    T* newElement_imp(Args... args) {
        T* ptr = nullptr;
        // ���ptr��Ϊ�������operator new��ʼ��
        if ((ptr = reinterpret_cast<T*>(MemoryManager::instance().allocate(sizeof(T)))) != nullptr) {
            new(ptr) T(std::forward<Args>(args)...);
        }
        return ptr;
    }
    // �ͷ��ڴ���ڴ�صĽӿ�
    template<typename T>
    void deleteElement(T* ptr) {
        if (ptr != nullptr) {
            ptr->~T();
            MemoryManager::instance().deallocate(ptr);
            DEBUG_MEM_DEALLOCATE_BLOCK(sizeof(T), ptr);
        }
    }
} // TinyRenderer

