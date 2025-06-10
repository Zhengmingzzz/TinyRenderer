//
// Created by Administrator on 25-4-23.
//

#include "DebugMemoryManager.h"
#include <cmath>
#include "Core/MemoryManager/MemoryManager.h"

namespace TinyRenderer {
    DebugMemoryManager& DebugMemoryManager::get_instance() {
        static DebugMemoryManager* instance = nullptr;
        if (instance == nullptr) {
            instance = new DebugMemoryManager();
        }
        return *instance;
    }

    void DebugMemoryManager::startup() {
        debug_allocators_.resize(MemoryManager::get_instance().MAX_TWOPOWERI+1);
    }


    void DebugMemoryManager::shutdown() {
        // // 记录结束时间
        // end_time_ = std::chrono::system_clock::now();
        // // 计算总的时间间隔
        // auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time_ - start_time_);
        // survival_time_ = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        //
        for (int i = 0; i < debug_allocators_.size(); i++) {
            debug_allocators_[i].on_shutdown();
        }

        // ordered_json mem_log_info = log_mem_info();
        // JsonStream json_stream;
        // Path log_page = FileServer::get_rootPath() / "logs" / "DebugMemoryLog" / "MemoryManager_log.json";
        // json_stream.save(std::move(log_page), mem_log_info);
    }

    void DebugMemoryManager::on_alloc_block(int alloc_idx,void* block, std::string&& file, size_t line) {
        debug_allocators_[alloc_idx].on_allocate_block(block, std::move(file), line);
        // total_alloc_cnt_++;
    }

    void DebugMemoryManager::on_dealloc_block(int block_size, void* block) {
        int allocator_idx = std::log2(block_size);
        debug_allocators_[allocator_idx].on_deallocate_block(block);
    }

    // void DebugMemoryManager::on_allocate_newPage(void* page, int block_size, int block_num) {
    //     int allocator_idx = std::log2(block_size);
    //     debug_allocators_[allocator_idx].on_allocate_newPage(page, block_size, block_num);
    // }

    // ordered_json DebugMemoryManager::log_mem_info() {
    //     ordered_json res;
    //     res["File"] = "Debug Memory Manager";
    //     res["allocate count"] = total_alloc_cnt_;
    //     res["survival time(seconds)"] = survival_time_;
    //     for (int i = 0; i < debug_allocators_.size(); i++) {
    //         std::ostringstream oss;
    //         oss << "Allocator " << i;
    //         res[oss.str()] = debug_allocators_[i].log_allocator_info();
    //     }
    //     return res;
    // }
} // TinyRenderer