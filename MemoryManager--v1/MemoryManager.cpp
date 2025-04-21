//
// Created by Administrator on 25-3-11.
//

#include "MemoryManager.h"
#include "Page.h"
#include <cstdlib>
#include "Function/Message/Message.h"

#include "Core/MemoryManager/PageManager.h"

namespace TinyRenderer {
    unsigned int MemoryManager::allocator_num;
    unsigned int MemoryManager::alignment = 8;
    unsigned int MemoryManager::max_userCapacity = 1024;
    bool MemoryManager::isEnable = false;
    unsigned char MemoryManager::mallocFlag = 0xff;

    void MemoryManager::startUp() {
		PageManager::instance()->startUp();
        startUp(alignment, max_userCapacity);
    }

    void MemoryManager::startUp(int alignment, unsigned int max_userCapacity) {
        // 计算出最多能有多少个allocator，最后一个allocator只能分配一个块，直接用malloc就行
        this->allocator_num = std::log(max_userCapacity) / log(2) - 1;
        this->startUp(alignment, this->allocator_num, max_userCapacity);
    }

    void MemoryManager::startUp(unsigned int alignment, unsigned int allocator_num, unsigned int max_userCapacity)
    {
        ASSERT(max_userCapacity % 2 == 0);
        // max_userCapacity 最大只能为2的14次方，大于则会让2字节块索引越界
        ASSERT(max_userCapacity <= 9182);
        this->alignment = alignment;
        this->allocator_num = allocator_num;
        this->max_userCapacity = max_userCapacity;

        allocator = static_cast<Allocator *>(malloc(sizeof(Allocator) * allocator_num));
        ASSERT(allocator != nullptr);
        for (int i=0; i < allocator_num; i++) 
        {
            allocator[i].startUp(i+1, max_userCapacity);
        }
        isEnable = true;
    }

    void MemoryManager::shutDown()
    {
		// 释放所有allocator中的所有page内存
        for (int i=0; i < allocator_num; i++) 
        {
            allocator[i].shutDown();
        }
        free(allocator);
        // 释放完所有page后再释放PageManager
        PageManager::instance()->shutDown();
        isEnable = false;
    }

    void* MemoryManager::Allocate(size_t size) 
    {
        // 是否第一个进入这个allocate函数，没有递归调用
        ASSERT(size > 0);

        // 多分配1字节用于存储pageID
        size += 1;

        // 查找size适合在哪一个分配器分配内存
        int alloc_idx = log(size) / log(2) - 1;
        for (; alloc_idx < allocator_num; alloc_idx++)
        {
            if (size <= allocator[alloc_idx].block_size) 
            {
                break;
            }
        }

        void* result = nullptr;
        // allocator_num表示allocator的数量，过大的内存分配和销毁都不会很频繁，所以直接使用malloc
        // TODO:竞态条件1 别的线程修改isEnable后，可能会有别的线程进入
        if (alloc_idx >= allocator_num || isEnable == false) 
        {
            result = malloc(size);
            unsigned char* uc_ptr = reinterpret_cast<unsigned char*>(result);
            // 把首个字节设置为最大值0xff表示使用malloc分配
            *uc_ptr = mallocFlag;
            result = uc_ptr + 1;
        }
        else 
        {
            result = allocator[alloc_idx].Allocate();
        }

        // 若分配后的返回值为空，则报错退出程序
        ASSERT(result != nullptr);
        return result;
    }

    void MemoryManager::Deallocate(void* ptr) {
        // 查看ptr上一字节，获取这个block对应的page
        unsigned char* uc_ptr = reinterpret_cast<unsigned char*>(ptr);
        if (uc_ptr[-1] == MemoryManager::mallocFlag) {
            free(reinterpret_cast<void*>(uc_ptr - 1));
            return;
        }

        // 当前收回的block所属的page
        Page* page = PageManager::instance()->SearchPage(uc_ptr[-1]);
        if (page == nullptr) {
            LOG_ERROR("can`t find page in pagemanager");
        }
        // 让收回page内存块
        page->Deallocate(uc_ptr - 1);

        ASSERT(page->allocator_idx <= allocator_num);
        // 通知allocator内存已经回收，修改数据
        Allocator* cur_alloc = allocator + page->allocator_idx;
        // 通知page对应的allocator回收了块
        cur_alloc->RecycleBlock();

        // 如果当前page空闲，没有块被引用，根据回收策略考虑是否回收
        if (page->block_num == page->max_block_num) {
            // 返回true则表示需要回收page
            if (PageManager::instance()->IsRecycle(cur_alloc) == true) {
                // 让allocator中的pagelist断开连接
                cur_alloc->RecyclePage(page);
                page->shutDown();
            }
        }
    }

    MemoryManager& MemoryManager::instance() {
        static MemoryManager instance;
        return instance;
    }


} // TinyRenderer