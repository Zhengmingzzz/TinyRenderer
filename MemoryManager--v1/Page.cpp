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
        // 记录总共的块数量
        this->max_block_num = blockNum_perPage;

        this->block_num= blockNum_perPage;
        // TODO:竞态条件6
        this->pageID = Page::pageNum++;
        ASSERT(Page::pageNum != 0xff)
        this->block_size = block_size;
        this->userMemory = userMemory;
        // TODO:竞态条件7 修改了freelist，可能会影响别的线程
        this->freelist = userMemory;
        // 当前page为新的page，所以指向下一个的指针应该置空
        this->next = nullptr;
        // 计算出当前allocator的索引值
        this->allocator_idx = log(block_size) / log(2);

        // 如果大于指针类型调用LinkBlock_Ptr
        // 否则调用LinkBlock_Idx
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
        // 初始化最后一个block,经过循环cur_block已经指向最后一个块
        cur_block->next=nullptr;
    }

    void Page::LinkBlock_Idx(unsigned int block_num, unsigned int block_size, void* address) {
        ASSERT(block_num > 0 && block_num < 0xffff);
        unsigned char* uc_address = reinterpret_cast<unsigned char*>(address);
        // 将block索引相连
        for (int i = 0;i < block_num - 1;i++) {
            Idx_block* cur_block = Idx_block::GetAddress(i, address, block_size);
            cur_block->next_block = i + 1;
        }
        // 最后一个块独自初始化
        Idx_block* last_block = reinterpret_cast<Idx_block*>(uc_address + block_size * (block_num - 1));
        // 最后一个块用0xffff表示后续没有别的块
        last_block->next_block = 0xffff;
    }

    void* Page::Allocate() {
        void* result = nullptr;
        // TODO:竞态条件4 freelist不允许多线程读写
        if (isPtrBlock) {
            // 将freelist解释为block类型
            Ptr_block* pb_freelist = reinterpret_cast<Ptr_block*>(this->freelist);
            result = this->freelist;
            this->freelist = pb_freelist->next;
        }
        else {
            // 将freelist解释为block类型
            Idx_block* ib_freelist = reinterpret_cast<Idx_block*>(this->freelist);
            result = this->freelist;
            // 根据索引获取下一个block的地址并为freelist重新赋值
            this->freelist = Idx_block::GetAddress(ib_freelist->next_block, userMemory, block_size);
        }
        if (result == nullptr) {
            LOG_ERROR("block is not enough pageID is:" + pageID);
        }

        // 空间前1字节处做标记
        unsigned char* blockHead = reinterpret_cast<unsigned char*>(result);
        *blockHead = this->pageID;
        // 返回result+1字节的地址
        result = reinterpret_cast<void*>(blockHead + 1);
        // TODO:竞态条件5 应该保持原子性
        block_num--;
        return result;
    }

    // 要收回的空间为ptr下的空间+上一字节的空间，需要使用block将这一整个空间覆盖并链接至freelist
    void Page::Deallocate(void* rawAddress) {
        // 判断block类型，获取page中blocklist并将新block链接起来
        // 使用头插法,获取首个block
        if (isPtrBlock) {
            Ptr_block* blocklist = reinterpret_cast<Ptr_block*>(this->freelist);
            // 把需要收回的块解释为block类型，并执行链接操作
            Ptr_block* cur_block = reinterpret_cast<Ptr_block*>(rawAddress);
            cur_block->next = blocklist;
            // 更新page的freelist
            this->freelist = cur_block;
        }
        // 使用头插法
        else {
            // 将要释放的地址解释为block类型
            Idx_block* cur_block = reinterpret_cast<Idx_block*>(rawAddress); 
            // 将需要释放的地址链接至末尾
            cur_block->next_block = Idx_block::GetIdx(this->freelist, userMemory, block_size);
            // 更改Page的blocklist指针
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