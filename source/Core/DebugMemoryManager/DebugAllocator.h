//
// Created by Administrator on 25-4-23.
//
#pragma once
#include <string>
#include <unordered_map>
#include "Function/CommonType/json.h"

namespace TinyRenderer {

    class DebugAllocator {
    public:
        int block_size_;
        int min_blockNum_perPage_;
        int max_blockNum_perPage_;
        size_t overflow_cnt; // total_allocate_cnt的溢出次数
        size_t total_allocate_cnt_; // 记录总共申请的次数
        size_t alloc_new_page_cnt_; // 记录申请新页的次数 （总共申请的次数-申请新页的次数 = 命中的次数）

    public:
        void on_startUp(int block_size, int min_block_num_perPage, int max_block_num_perPage);
        void on_shutdown();
        void on_allocate_block(void* block_addr, std::string&& file, size_t line);
        void on_deallocate_block(void* page);
        void on_allocate_newPage(void* page, int block_size, int block_num);
        void get_leak_memory();
        ordered_json log_allocator_info();

    private:
        using P_SS = std::pair<std::string, size_t>;
        std::unordered_map<void*, P_SS> block_info_; // 记录每个内存块的申请地址
    };

} // TinyRenderer