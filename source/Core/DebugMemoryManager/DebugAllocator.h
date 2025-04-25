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
        size_t overflow_cnt; // total_allocate_cnt���������
        size_t total_allocate_cnt_; // ��¼�ܹ�����Ĵ���
        size_t alloc_new_page_cnt_; // ��¼������ҳ�Ĵ��� ���ܹ�����Ĵ���-������ҳ�Ĵ��� = ���еĴ�����

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
        std::unordered_map<void*, P_SS> block_info_; // ��¼ÿ���ڴ��������ַ
    };

} // TinyRenderer