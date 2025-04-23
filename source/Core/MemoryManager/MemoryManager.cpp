//
// Created by Administrator on 25-4-15.
//

#include "MemoryManager.h"

#include <cmath>
#include <memory>
#include <mutex>
#include <glm/exponential.hpp>
#include <windows.h>
#include <psapi.h>

namespace TinyRenderer {

    MemoryManager& MemoryManager::instance() {
        static MemoryManager* instance = nullptr;
        std::call_once(instance_flag_, [&]() {
            instance = new MemoryManager();
        });
        return *instance;
    }


    void MemoryManager::startUp(int min_blockNum_perPage) {
        MemoryManager& memoryManager = MemoryManager::instance();
        // 从2的0次方一直到2的MAX_TWOPOWERI次方
        size_t block_size = 1;
        for (int i = 0; i <= MAX_TWOPOWERI; i++) {
            allocators_[i] = std::make_unique<Allocator>();
            allocators_[i]->startUp(block_size, MIN_BLOCKNUM_PERPAGE, (MAX_TWOPOWERI - i + 1) * 2); // 每个allocator根据分配块的大小，限制它的每页最大块数
            block_size <<= 1;
        }

        // 初始化平滑系数和平均值
        ewma_occupancy_.smoothing_factor_=0.7f;
        ewma_occupancy_.average_occupancy_=0.0f;

        occupancy_level_ = Occupancy_level::low; // 一开始先设置低级别的内存占用率
    }

    void MemoryManager::shutDown() {
        MemoryManager& memoryManager = MemoryManager::instance();
        for (int i = 0; i <= MAX_TWOPOWERI; i++) {
            allocators_[i]->shutDown();
            allocators_[i].release();
        }
        delete &memoryManager;
        DEBUG_MEM_SHUTDOWN();
    }

    void* MemoryManager::allocate(size_t size) {
        ASSERT(size>0)
        StopWatch_Start(alloc_time);

        size_t twopoweri = MAX_TWOPOWERI + 1;
        // 找到大于size的第一个allocator
        for (int i=0;i<=MAX_TWOPOWERI;i++) {
            if (allocators_[i]->block_size_<=size) {
                twopoweri = i;
            }
            else
                break;
        }

        void* res = nullptr;
        // 超出内存池管理范围，直接使用malloc
        if (twopoweri > MAX_TWOPOWERI) {
            res = malloc(twopoweri);
        }
        else {
            res = allocators_[twopoweri]->allocate();
        }
        StopWatch_Pause(alloc_time);
        return res;
    }

    void MemoryManager::deallocate(void* ptr) {
        StopWatch_Start(deallocate_time);
        // 1.找到小于ptr的最大值
        auto it = pages_.upper_bound(reinterpret_cast<Page*>(ptr));
        if (it != pages_.begin()) {
            --it;
            Page* page = *it;
            bool isRecycle_page = false;
            // 2.如果try_dellocate返回true代表回收成功
            if (page->owner_->try_dellocate(page, reinterpret_cast<Block*>(ptr), isRecycle_page)) {
                // 如果删除了当前的page则去除pages的page数据
                if (isRecycle_page) {
                    pages_.erase(it);
                }
                StopWatch_Pause(deallocate_time);
                return;
            }
        }
        // 如果到这没有回收则调用free
        free(ptr);
        StopWatch_Pause(deallocate_time);
    }

    void MemoryManager::tick() {
        // 每refresh_interval_次循环一次
        static int frame_counter = 0;
        frame_counter++;

        for (int i=0;i<=MAX_TWOPOWERI;i++) {
            allocators_[i]->tick();
        }
        // 如果帧计数器为0，触发刷新指标逻辑
        if (frame_counter >= refresh_interval_) {
            refreshMetrics();
            for (int i=0;i<=MAX_TWOPOWERI;i++) {
                allocators_[i]->refreshMetrics(occupancy_level_);
            }
        }
        frame_counter %= refresh_interval_;
    }

    void MemoryManager::refreshMetrics() {
        // 获取系统虚拟内存占用率
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            SIZE_T processVirtualUsed = pmc.PagefileUsage;

            // 获取系统总虚拟内存
            MEMORYSTATUSEX memStatus;
            memStatus.dwLength = sizeof(memStatus);
            if (GlobalMemoryStatusEx(&memStatus)) {
                DWORDLONG totalVirtual = memStatus.ullTotalPageFile;

                // 计算并输出比值
                double ratio = static_cast<double>(processVirtualUsed) / totalVirtual;
                ratio *= 100;
                // 根据比值更改占用率
                if (ratio < (double)Occupancy_level::medium) {
                    occupancy_level_ = Occupancy_level::low;
                    refresh_interval_ = 15;
                }
                else if (ratio > (double)Occupancy_level::high) {
                    occupancy_level_ = Occupancy_level::high;
                    refresh_interval_ = 10;
                }
                else {
                    occupancy_level_ = Occupancy_level::medium;
                    refresh_interval_ = 5;
                }
            }
        }
        else {
            LOG_ERROR("system Memory occupancy calculate occur error!");
        }


        for (int i=0;i<=MAX_TWOPOWERI;i++) {
            allocators_[i]->refreshMetrics(occupancy_level_);
        }
    }

    void MemoryManager::register_newPage(Page* new_page) {
        pages_.insert(new_page);
    }

} // TinyRenderer