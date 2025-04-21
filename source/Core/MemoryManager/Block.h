//
// Created by Administrator on 25-4-16.
//
#pragma once
#include <cstdint>
#include "Function/Message/Message.h"

namespace TinyRenderer {

    class Block {
    public:
        unsigned char next_idx_; // ʹ��ԭ�ӱ�����֤ԭ���ԣ���ֹ����߳�ͬʱ�޸�

        // ���㵱ǰ�������ֵ
        unsigned int get_index(void* user_memory, unsigned int block_size) {
            ASSERT(user_memory != nullptr);
            ASSERT(block_size > 0);
            ASSERT(user_memory <= this);

            unsigned char res = 0xff;
            uintptr_t head_addr = reinterpret_cast<uintptr_t>(user_memory);
            uintptr_t cur_addr = reinterpret_cast<uintptr_t>(this);
            if (cur_addr >= head_addr)
                res = (cur_addr - head_addr) / block_size;
            return res;
        }
        // ������Ϣ����index����׵�ַ
        static Block* get_block(void* user_memory, unsigned int index, unsigned int block_size, unsigned int max_index) {
            ASSERT(user_memory != nullptr);
            ASSERT(block_size > 0);
            ASSERT(index >= 0);
            if (index > max_index)
                return nullptr;
            uintptr_t head_addr = reinterpret_cast<uintptr_t>(user_memory);
            Block* res = reinterpret_cast<Block*>(head_addr + index*block_size);
            return res;
        }
    };

} // TinyRenderer

