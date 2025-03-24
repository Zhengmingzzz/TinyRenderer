//
// Created by Administrator on 25-3-11.
//

#include "MemoryManager.h"
#include "Page.h"
#include <cstdlib>
#include "Function/Message/Message.h"

#include "Core/MemoryManager/PageManager.h"

namespace TinyRenderer {
    // ����Ϊ2�Ĵη���

    unsigned int MemoryManager::allocator_num;
    unsigned int MemoryManager::alignment = 8;
    unsigned int MemoryManager::max_userCapacity = 1024;
    bool MemoryManager::isEnable = false;
    unsigned char MemoryManager::mallocFlag = 0xff;

    void MemoryManager::StartUp() {
		PageManager::GetInstance()->StartUp();
        StartUp(alignment, max_userCapacity);
    }

    void MemoryManager::StartUp(unsigned int alignment, unsigned int allocator_num, unsigned int max_userCapacity) 
    {
        ASSERT(max_userCapacity % 2 == 0);
        // max_userCapacity ���ֻ��Ϊ2��14�η������������2�ֽڿ�����ʧЧ
        ASSERT(max_userCapacity <= 9182);
        this->alignment = alignment;
        this->allocator_num = allocator_num;
        this->max_userCapacity = max_userCapacity;

        allocator = static_cast<Allocator *>(malloc(sizeof(Allocator) * allocator_num));
        ASSERT(allocator != nullptr);
        for (int i=0; i < allocator_num; i++) 
        {
            allocator[i].StartUp(i+1, max_userCapacity);
        }
        isEnable = true;
    }

    void MemoryManager::ShutDown() 
    {
		// �ͷ�����allocator�е�����page�ڴ�
        for (int i=0; i < allocator_num; i++) 
        {
            allocator[i].ShutDown();
        }
        free(allocator);
        // �ͷ�������page�����ͷ�PageManager
        PageManager::GetInstance()->ShutDown();
        isEnable = false;
    }

    void* MemoryManager::Allocate(size_t size) 
    {
        ASSERT(size > 0);

        // �����1�ֽ����ڴ洢pageID
        size += 1;

        // ����size�ʺ�����һ�������������ڴ�
        int alloc_idx = log(size) / log(2) - 1;
        for (; alloc_idx <allocator_num; alloc_idx++) 
        {
            if (size <= allocator[alloc_idx].block_size) 
            {
                break;
            }
        }
        void* result = nullptr;
        // allocator_num��ʾallocator��������������ڴ��������ٶ������Ƶ��������ֱ��ʹ��malloc
        if (alloc_idx >= allocator_num || isEnable == false) 
        {
            result = malloc(size);
            unsigned char* uc_ptr = reinterpret_cast<unsigned char*>(result);
            // ���׸��ֽ�����Ϊ���ֵ0xff��ʾʹ��malloc����
            *uc_ptr = mallocFlag;
            result = uc_ptr + 1;
        }
        else 
        {
            result = allocator[alloc_idx].Allocate();
        }

        ASSERT(result != nullptr);
        return result;
    }

    void MemoryManager::Deallocate(void* ptr) {
        // �鿴ptr��һ�ֽڣ���ȡ���block��Ӧ��page
        unsigned char* uc_ptr = reinterpret_cast<unsigned char*>(ptr);
        if (uc_ptr[-1] == MemoryManager::mallocFlag) {
            free(reinterpret_cast<void*>(uc_ptr - 1));
            return;
        }

        // ��ǰ�ջص�block������page
        Page* page = PageManager::GetInstance()->SearchPage(uc_ptr[-1]);
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