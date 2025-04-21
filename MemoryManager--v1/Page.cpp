//
// Created by Administrator on 25-3-11.
//

#include "Page.h"
#include <cmath>
#include "PageManager.h"
#include "Function/Message/Message.h"
#include "Block.h"

namespace TinyRenderer {
    unsigned char Page::pageNum = 0;

    void Page::startUp(unsigned int blockNum_perPage, unsigned int block_size, void* userMemory) {
        // ��¼�ܹ��Ŀ�����
        this->max_block_num = blockNum_perPage;

        this->block_num= blockNum_perPage;
        // TODO:��̬����6
        this->pageID = Page::pageNum++;
        ASSERT(Page::pageNum != 0xff)
        this->block_size = block_size;
        this->userMemory = userMemory;
        // TODO:��̬����7 �޸���freelist�����ܻ�Ӱ�����߳�
        this->freelist = userMemory;
        // ��ǰpageΪ�µ�page������ָ����һ����ָ��Ӧ���ÿ�
        this->next = nullptr;
        // �������ǰallocator������ֵ
        this->allocator_idx = log(block_size) / log(2);

        // �������ָ�����͵���LinkBlock_Ptr
        // �������LinkBlock_Idx
        if (block_size >= sizeof(void*)) {
            LinkBlock_Ptr(blockNum_perPage, block_size, userMemory);
            this->isPtrBlock = true;
        }
        else {
            LinkBlock_Idx(blockNum_perPage, block_size, userMemory);
            this->isPtrBlock = false;
        }
    }

    void Page::shutDown() {
        PageManager::instance()->FreePage(pageID);
    }

    void Page::LinkBlock_Ptr(unsigned int block_num, unsigned int block_size, void* address) {
        unsigned char* uc_address = reinterpret_cast<unsigned char*>(address);

        Ptr_block* cur_block = reinterpret_cast<Ptr_block*>(address);
        cur_block->next=nullptr;
        Ptr_block* next_block = nullptr;

        for (int i = 1;i < block_num; i++) {
            next_block = reinterpret_cast<Ptr_block *>(uc_address + i * block_size);
            cur_block->next=next_block;
            cur_block = next_block;
        }
        // ��ʼ�����һ��block,����ѭ��cur_block�Ѿ�ָ�����һ����
        cur_block->next=nullptr;
    }

    void Page::LinkBlock_Idx(unsigned int block_num, unsigned int block_size, void* address) {
        ASSERT(block_num > 0 && block_num < 0xffff);
        unsigned char* uc_address = reinterpret_cast<unsigned char*>(address);
        // ��block��������
        for (int i = 0;i < block_num - 1;i++) {
            Idx_block* cur_block = Idx_block::GetAddress(i, address, block_size);
            cur_block->next_block = i + 1;
        }
        // ���һ������Գ�ʼ��
        Idx_block* last_block = reinterpret_cast<Idx_block*>(uc_address + block_size * (block_num - 1));
        // ���һ������0xffff��ʾ����û�б�Ŀ�
        last_block->next_block = 0xffff;
    }

    void* Page::Allocate() {
        void* result = nullptr;
        // TODO:��̬����4 freelist��������̶߳�д
        if (isPtrBlock) {
            // ��freelist����Ϊblock����
            Ptr_block* pb_freelist = reinterpret_cast<Ptr_block*>(this->freelist);
            result = this->freelist;
            this->freelist = pb_freelist->next;
        }
        else {
            // ��freelist����Ϊblock����
            Idx_block* ib_freelist = reinterpret_cast<Idx_block*>(this->freelist);
            result = this->freelist;
            // ����������ȡ��һ��block�ĵ�ַ��Ϊfreelist���¸�ֵ
            this->freelist = Idx_block::GetAddress(ib_freelist->next_block, userMemory, block_size);
        }
        if (result == nullptr) {
            LOG_ERROR("block is not enough pageID is:" + pageID);
        }

        // �ռ�ǰ1�ֽڴ������
        unsigned char* blockHead = reinterpret_cast<unsigned char*>(result);
        *blockHead = this->pageID;
        // ����result+1�ֽڵĵ�ַ
        result = reinterpret_cast<void*>(blockHead + 1);
        // TODO:��̬����5 Ӧ�ñ���ԭ����
        block_num--;
        return result;
    }

    // Ҫ�ջصĿռ�Ϊptr�µĿռ�+��һ�ֽڵĿռ䣬��Ҫʹ��block����һ�����ռ串�ǲ�������freelist
    void Page::Deallocate(void* rawAddress) {
        // �ж�block���ͣ���ȡpage��blocklist������block��������
        // ʹ��ͷ�巨,��ȡ�׸�block
        if (isPtrBlock) {
            Ptr_block* blocklist = reinterpret_cast<Ptr_block*>(this->freelist);
            // ����Ҫ�ջصĿ����Ϊblock���ͣ���ִ�����Ӳ���
            Ptr_block* cur_block = reinterpret_cast<Ptr_block*>(rawAddress);
            cur_block->next = blocklist;
            // ����page��freelist
            this->freelist = cur_block;
        }
        // ʹ��ͷ�巨
        else {
            // ��Ҫ�ͷŵĵ�ַ����Ϊblock����
            Idx_block* cur_block = reinterpret_cast<Idx_block*>(rawAddress); 
            // ����Ҫ�ͷŵĵ�ַ������ĩβ
            cur_block->next_block = Idx_block::GetIdx(this->freelist, userMemory, block_size);
            // ����Page��blocklistָ��
            this->freelist = cur_block;
        }
        block_num++;
    }

    Page* Page::GetNewPage(unsigned int block_size, unsigned int blockNum_perPage, unsigned int alignment) {
        ASSERT(block_size > 0);
        ASSERT(blockNum_perPage > 0);
        ASSERT(block_size % 2 == 0);
        return PageManager::instance()->AllocNewPage(block_size, blockNum_perPage,alignment);
    }


} // TinyRenderer