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
        // �����������ж��ٸ�allocator�����һ��allocatorֻ�ܷ���һ���飬ֱ����malloc����
        this->allocator_num = std::log(max_userCapacity) / log(2) - 1;
        this->startUp(alignment, this->allocator_num, max_userCapacity);
    }

    void MemoryManager::startUp(unsigned int alignment, unsigned int allocator_num, unsigned int max_userCapacity)
    {
        ASSERT(max_userCapacity % 2 == 0);
        // max_userCapacity ���ֻ��Ϊ2��14�η������������2�ֽڿ�����Խ��
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
		// �ͷ�����allocator�е�����page�ڴ�
        for (int i=0; i < allocator_num; i++) 
        {
            allocator[i].shutDown();
        }
        free(allocator);
        // �ͷ�������page�����ͷ�PageManager
        PageManager::instance()->shutDown();
        isEnable = false;
    }

    void* MemoryManager::Allocate(size_t size) 
    {
        // �Ƿ��һ���������allocate������û�еݹ����
        ASSERT(size > 0);

        // �����1�ֽ����ڴ洢pageID
        size += 1;

        // ����size�ʺ�����һ�������������ڴ�
        int alloc_idx = log(size) / log(2) - 1;
        for (; alloc_idx < allocator_num; alloc_idx++)
        {
            if (size <= allocator[alloc_idx].block_size) 
            {
                break;
            }
        }

        void* result = nullptr;
        // allocator_num��ʾallocator��������������ڴ��������ٶ������Ƶ��������ֱ��ʹ��malloc
        // TODO:��̬����1 ����߳��޸�isEnable�󣬿��ܻ��б���߳̽���
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

        // �������ķ���ֵΪ�գ��򱨴��˳�����
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
        Page* page = PageManager::instance()->SearchPage(uc_ptr[-1]);
        if (page == nullptr) {
            LOG_ERROR("can`t find page in pagemanager");
        }
        // ���ջ�page�ڴ��
        page->Deallocate(uc_ptr - 1);

        ASSERT(page->allocator_idx <= allocator_num);
        // ֪ͨallocator�ڴ��Ѿ����գ��޸�����
        Allocator* cur_alloc = allocator + page->allocator_idx;
        // ֪ͨpage��Ӧ��allocator�����˿�
        cur_alloc->RecycleBlock();

        // �����ǰpage���У�û�п鱻���ã����ݻ��ղ��Կ����Ƿ����
        if (page->block_num == page->max_block_num) {
            // ����true���ʾ��Ҫ����page
            if (PageManager::instance()->IsRecycle(cur_alloc) == true) {
                // ��allocator�е�pagelist�Ͽ�����
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