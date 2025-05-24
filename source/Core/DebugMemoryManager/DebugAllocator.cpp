//
// Created by Administrator on 25-4-23.
//

#include "DebugAllocator.h"
#include <limits>
#include <cmath>
#include "Function/Message/Message.h"

namespace TinyRenderer {
    void DebugAllocator::on_shutdown() {
        // 查看内存泄露
        get_leak_memory();
        block_info_.clear();
    }

    void DebugAllocator::on_allocate_block(void* block_addr, std::string&& file, size_t line) {
        block_info_.emplace(block_addr, std::make_pair(file, line));
    }


    void DebugAllocator::on_deallocate_block(void *block) {
        block_info_.erase(block);
    }

    void DebugAllocator::get_leak_memory() {
        for (auto& block_info : block_info_) {
            P_SS& pss = block_info.second;
            LOG_WARN("memory leak detected " << pss.first << ":" << pss.second);
        }
    }
} // TinyRenderer