//
// Created by Administrator on 25-3-11.
//
#pragma once
#include "Block.h"

namespace TinyRenderer {

class Page {
public:
    friend class PageManager;
    bool isPtrBlock;
    // 剩余block数量
    unsigned int block_num;
    // 本页block
    unsigned int block_size;
    Page* next;
public:
    static Page* GetNewPage(unsigned int block_size, unsigned int blockNum_perPage, unsigned int alignment);

    void StartUp(unsigned int blockNum_perPage, unsigned int block_size, void* userMemory);
    void ShutDown();

    void* Allocate();
    void Deallocate(void* rawAddress);
private:
    // 内存块的上1字节用于存储pageID，所以pageNum最大不超过0xff
    static unsigned char pageNum;
    // page的唯一识别号
    unsigned char pageID;
    // 用户空间距离首地址的偏移量
    void* userMemory;
    // 首个block块的地址，block有两种类型，因此用void*代替
    // 用于在Page.Deallocate中解释为特定类型并链接
    void* freelist;

    // 用于初始化时链接块,block_num表示从1开始的块的个数,ptr代表整个空间的首地址
    static void LinkBlock_Ptr(unsigned int block_num,unsigned int block_size, void* ptr);
    static void LinkBlock_Idx(unsigned int block_num, unsigned int block_size, void* ptr);
};

} // TinyRenderer
