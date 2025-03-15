//
// Created by Administrator on 25-3-11.
//

#include "MemoryManager.h"
#include "Page.h"
#include <cstdlib>
#include "Function/Message/Message.h"

#include "Function/MemoryManager/PageManager.h"

namespace TinyRenderer {
    unsigned int MemoryManager::allocator_num;
    unsigned int MemoryManager::alignment = 8;
    unsigned int MemoryManager::max_userCapacity = 1024;
    bool MemoryManager::isEnable = false;
    unsigned char MemoryManager::mallocFlag = 0xff;

    void MemoryManager::StartUp() {
        StartUp(alignment, max_userCapacity);
    }

    void MemoryManager::StartUp(unsigned int alignment, unsigned int max_userCapacity) {
        // 计算出最多能有多少个allocator
        this->allocator_num = std::log(max_userCapacity) / log(2) - 1;
        this->StartUp(alignment,this->allocator_num, max_userCapacity);
    }

    void MemoryManager::StartUp(unsigned int alignment, unsigned int allocator_num, unsigned int max_userCapacity) {
        ASSERT(max_userCapacity % 2 == 0)
        this->alignment = alignment;
        this->allocator_num = allocator_num;
        this->max_userCapacity = max_userCapacity;

        allocator = static_cast<Allocator *>(malloc(sizeof(Allocator) * allocator_num));
        ASSERT(allocator != nullptr);
        for (int i=0; i < allocator_num; i++) {
            allocator[i].StartUp(i+1, max_userCapacity);
        }
        isEnable = true;
    }

    void MemoryManager::ShutDown() {
        for (int i=0; i < allocator_num; i++) {
            allocator[i].ShutDown();
        }
        free(allocator);
        isEnable = false;
    }

    void* MemoryManager::Allocate(size_t size) {
        ASSERT(size > 0);
        // 如果在一开始MamoryManager还没初始化时就申请内存则直接使用malloc
        if (isEnable == false) {
            void* result = malloc(size + 1);
            unsigned char* uc_ptr = reinterpret_cast<unsigned char*>(result);
            // 把首个字节设置为最大值0xff表示使用malloc分配
            *uc_ptr = mallocFlag;
            result = uc_ptr + 1;
            return result;
        }
        // 多分配1字节用于存储pageID
        size += 1;
        int i=0;
        for (; i<allocator_num; i++) {
            if (size <= allocator[i].block_size) {
                break;
            }
        }
        void* result = nullptr;
        // allocator_num表示allocator的数量，过大的内存分配和销毁都不会很频繁，所以直接使用malloc
        if (i >= allocator_num) {
            result = malloc(size);
            unsigned char* uc_ptr = reinterpret_cast<unsigned char*>(result);
            // 把首个字节设置为最大值0xff表示使用malloc分配
            *uc_ptr = mallocFlag;
            result = uc_ptr + 1;
        }
        else {
            result = allocator[i].Allocate();
        }

        ASSERT(result != nullptr);
        return result;
    }

    void MemoryManager::Deallocate(void* ptr) {
        // 查看ptr上一字节，获取这个block对应的page
        unsigned char* uc_ptr = reinterpret_cast<unsigned char*>(ptr);
        if (uc_ptr[-1] == PageManager::mallocFlag) {
            free(reinterpret_cast<void*>(uc_ptr - 1));
            return;
        }

        // 当前收回的block所属的page
        Page* page = PageManager::GetInstance().SearchPage(uc_ptr[-1]);
        if (page == nullptr) {
            LOG_ERROR("page is nullptr");
        }

        page->Deallocate(uc_ptr - 1);
    }

    MemoryManager& MemoryManager::GetInstance() {
        static MemoryManager instance;
        return instance;
    }


} // TinyRenderer