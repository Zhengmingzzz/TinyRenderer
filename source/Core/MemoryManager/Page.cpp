//
// Created by Administrator on 25-4-16.
//

#include "Page.h"
#include "Function/Message/Message.h"
#include "Block.h"
#include "MemoryManager.h"

namespace TinyRenderer {
    void Page::startUp(int block_size, int block_num, void* user_address, Allocator* owner) {
        ASSERT(block_size > 0);
        ASSERT(block_num > 0);
        ASSERT(block_num < END_MARKER)
        ASSERT(user_address != nullptr);

        this->current_block_num_ = block_num;
        this->total_block_num_ = block_num;
        this->block_size_ = block_size;
        this->freelist_ = reinterpret_cast<Block*>(user_address);
        this->user_address_ = reinterpret_cast<Block*>(user_address);
        this->next = nullptr;
        this->prev = nullptr;
        owner_ = owner;

        // ����ÿ�����п�
        Page::link_freelist(user_address_, block_size, block_num);
    }

    void Page::shutDown() {
        ASSERT(this->current_block_num_ == this->total_block_num_);
    }

    void Page::link_freelist(Block *user_address, int block_size, int block_num) {
        ASSERT(block_num > 0 && block_num < END_MARKER);

        Block* cur_block = nullptr;
        for (int i=0;i<block_num;i++) {
            cur_block = Block::get_block(user_address, i, block_size, block_num - 1);
            cur_block->next_idx_ = i+1;
        }
        if (cur_block)
            cur_block->next_idx_ = END_MARKER;
    }


    Page *Page::allocate_newPage(int block_size, int block_num, Allocator* owner, int alignment) {
        // ����Ϊ2����
        ASSERT((alignment & (alignment - 1)) == 0);
        Page* new_page = nullptr;

        size_t extension_size = sizeof(Page) + alignment + (block_size * block_num);
        void* malloc_ptr = malloc(extension_size);
        ASSERT(malloc_ptr != nullptr);

        // ͨ��λ��������������û��ռ��׵�ַ
        uintptr_t raw_address = reinterpret_cast<uintptr_t>(malloc_ptr) + sizeof(Page);
        unsigned int mask = alignment - 1;
        unsigned int misalignment = (raw_address & mask);
        unsigned int offset = alignment - misalignment;
        void* aligned_address = reinterpret_cast<void*>(raw_address + offset);

        // ��ʼ��new_page����
        new_page = new (malloc_ptr) Page;

        if (new_page != nullptr) {
            new_page->startUp(block_size, block_num, aligned_address, owner);
        }

        // ��MemoryManagerע����ҳ
        MemoryManager::instance().register_newPage(new_page);

        return new_page;
    }

    bool Page::try_allocate_block(void *&res) {
        StopWatch_Start(mem_try_allocate_time);
        // �˴������freelist_������
        //std::lock_guard<SpinLock> spin_lock_guard(page_spinlock_);
        if (!is_block_available()) {
            Counter_Add(mem_failed_allocate_block);
            StopWatch_Pause(mem_try_allocate_time);
            return false;
        }

        Counter_Add(mem_successful_allocate_block);
        StopWatch_Start(mem_getblock_time);
        // ����ʱ�Զ�����������ʱ�Զ�����
        Block* old_freelist = freelist_;
        Block* new_freelist = Block::get_block(user_address_, old_freelist->next_idx_, block_size_,this->total_block_num_ - 1);
        StopWatch_Pause(mem_getblock_time);

        if (freelist_->next_idx_ == old_freelist->next_idx_) {
            freelist_ = new_freelist;
            current_block_num_--;
            res = old_freelist;
        }

        StopWatch_Pause(mem_try_allocate_time);
        return true;
    }

    // ��������freelist_
    bool Page::try_deallocate_block(Block* block) {
        // ����freelist_ʱ������
        std::lock_guard spin_lock_guard(page_spinlock_);

        Block* freelist = freelist_;
        unsigned char freelist_idx = END_MARKER;
        // �����ʱfreelist��Ϊ��,�����freelist_idx
        if (freelist != nullptr)
            freelist_idx = freelist->get_index(this->user_address_, this->block_size_);
        // ʹ��ͷ�巨�����µĽڵ�
        block->next_idx_ = freelist_idx;
        freelist_ = block;
        current_block_num_ += 1;

        return true;
    }
} // TinyRenderer