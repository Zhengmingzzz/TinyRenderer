//
// Created by Administrator on 25-3-11.
//

#include "Allocator.h"

#include <cmath>

#include "MemoryManager.h"
#include "Function/Message/Message.h"

namespace TinyRenderer {

    void Allocator::StartUp(int twoPowerI, unsigned int max_userCapacity) {
        ASSERT(max_userCapacity > 0);
        ASSERT(twoPowerI > 0);
        pagelist = nullptr;

        this->block_size=static_cast<unsigned int>(pow(2, twoPowerI));
        // MAX_PAGESIZE和block_size都为2的i次方
        blockNum_prePage = block_size >= max_userCapacity ? 1 : max_userCapacity / block_size;
        if (block_size < sizeof(void*))
        {
            // 每页的最后一个块不用，用于表示下一个块为空索引
            blockNum_prePage = blockNum_prePage >= 0xffff ? 0xffff - 1 : blockNum_prePage;
        }
    }

    void Allocator::ShutDown() {
        FreeAllPage();
    }

    void Allocator::FreeAllPage() 
    {
        while (pagelist != nullptr) 
        {
            Page* next = pagelist->next;
            pagelist->ShutDown();
            pagelist = next;
        }
    }


    void* Allocator::Allocate() {
        void* result = nullptr;
        Page* page = pagelist;

        while (result == nullptr) {
            // 若当前也用完了或者首次使用page，则分配一个页,并头插进pagelist中
            if (page == nullptr || page->block_num == 0) {
                page = this->GetNewPage(block_size, blockNum_prePage,MemoryManager::alignment);
                ASSERT(page != nullptr);
                page->next = pagelist;
            }

            ASSERT(page != nullptr);
            result = page->Allocate();
            // 如果当前页没有空余的块,则查找下一个页
            if (result == nullptr) {
                ASSERT("address memory is filled");
            }
        }

        pagelist = page;

        return result;
    }

    Page* Allocator::GetNewPage(unsigned int block_size, unsigned int block_num_per_page, unsigned int alignment) {
        return Page::GetNewPage(block_size, blockNum_prePage,alignment);
    }

} // TinyRenderer