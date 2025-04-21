//
// Created by Administrator on 25-4-16.
//
#pragma once

#include "Core/Lock/SpinLock.h"
#include "Function/Message/Message.h"


namespace TinyRenderer {

    class Block;
    class Allocator;
    class Page {
    public:
        static inline constexpr int DEFAULT_ALIGNMENT = 8;
        Page* next;
        Page* prev;
        Block* freelist_;
        int current_block_num_;
        int total_block_num_;
        int block_size_;

        Allocator* owner_;
    private:
        static inline constexpr uint8_t END_MARKER = 0xff;
        SpinLock page_spinlock_; // 操作freelist_时(分配/释放内存)，需要上锁
        Block* user_address_; // 内存申请时分配的地址


    public:
        void startUp(int block_size, int block_num,  void* user_address, Allocator* owner_);
        void shutDown();
        static Page* allocate_newPage(int block_size, int block_num, Allocator* owner_, int alignment = DEFAULT_ALIGNMENT);

        bool try_allocate_block(void*& res);
        bool try_deallocate_block(Block* block);
        // 判断block地址是否属于这个页
        inline bool is_belongTo(Block* block) {
            ASSERT(block != nullptr);
            //1.判断block是否属于当前page
            std::uintptr_t block_address = reinterpret_cast<uintptr_t>(block);
            std::uintptr_t user_address = reinterpret_cast<uintptr_t>(this->user_address_);
            if (!(block_address >= user_address && block_address <= user_address + block_size_ * total_block_num_ - 1)) {
                return false;
            }
            return true;
        }

        // 判断是否有剩余块
        inline bool is_block_available() {
            return current_block_num_ > 0;
        }

    private:
        static void link_freelist(Block* user_address, int block_size, int block_num); // 提供首地址，每个块的大小和数量，连接每个块
    };

} // TinyRenderer

