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
    public:
        void on_shutdown();
        void on_allocate_block(void* block_addr, std::string&& file, size_t line);
        void on_deallocate_block(void* page);
        void get_leak_memory();

    private:
        using P_SS = std::pair<std::string, size_t>;
        std::unordered_map<void*, P_SS> block_info_; // 记录每个内存块的申请地址
    };

} // TinyRenderer