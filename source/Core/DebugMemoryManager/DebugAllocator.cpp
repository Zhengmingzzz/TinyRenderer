//
// Created by Administrator on 25-4-23.
//

#include "DebugAllocator.h"
#include <limits>
#include <cmath>
#include "Function/Message/Message.h"

namespace TinyRenderer {
    void DebugAllocator::on_startUp(int block_size, int min_block_num_perPage, int max_block_num_perPage) {
        this->block_size_ = block_size;
        this->min_blockNum_perPage_ = min_block_num_perPage;
        this->max_blockNum_perPage_ = max_block_num_perPage;
        this->total_allocate_cnt_ = 0;
    }

    void DebugAllocator::on_shutdown() {
        // ²é¿´ÄÚ´æÐ¹Â¶
        get_leak_memory();
        block_info_.clear();
    }

    void DebugAllocator::on_allocate_block(void* block_addr, std::string&& file, size_t line) {
        block_info_.emplace(block_addr, std::make_pair(file, line));
        total_allocate_cnt_++;
        if (total_allocate_cnt_ == std::numeric_limits<size_t>::max()) {
            overflow_cnt++;
            total_allocate_cnt_ = 0;
        }
    }


    void DebugAllocator::on_deallocate_block(void *block) {
        block_info_.erase(block);
    }

    void DebugAllocator::on_allocate_newPage(void *page, int block_size, int block_num) {
        alloc_new_page_cnt_++;
    }

    void DebugAllocator::get_leak_memory() {
        for (auto& block_info : block_info_) {
            P_SS& pss = block_info.second;
            LOG_WARN("memory leak detected " << pss.first << ":" << pss.second);
        }
    }

    ordered_json DebugAllocator::log_allocator_info() {
        ordered_json res;
        res["BlockSize"] = block_size_;
        res["MinBlockNumPerPage"] = min_blockNum_perPage_;
        res["MaxBlockNumPerPage"] = max_blockNum_perPage_;
        res["OverFlowCnt"] = overflow_cnt;
        res["TotalAllocatedCnt"] = total_allocate_cnt_;

        double hit_rate = total_allocate_cnt_ == 0 ? 0 : static_cast<double>(total_allocate_cnt_ - alloc_new_page_cnt_) / total_allocate_cnt_ * 100;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << hit_rate << '%';
        res["Hit Rate"] = ss.str();

        return res;
    }
} // TinyRenderer