//
// Created by Administrator on 25-3-11.
//
#pragma once
#include "Page.h"

namespace TinyRenderer {
class Allocator {
public:
    // 这个分配器分配2的twoPowerI次方内存大小
    void StartUp(int twoPowerI, unsigned int max_pageSize);
    void ShutDown();

    // 只负责返回特点大小的内存块，不做别的操作
    void* Allocate();

    // 当前Allocator分配固定block_size大小的内存
    unsigned int block_size;
    // 每个页的block数量
    unsigned int blockNum_prePage;
private:
    Page* pagelist = nullptr;
    Page* GetNewPage(unsigned int block_size, unsigned int blockNum_prePage, unsigned int alignment);
    void FreeAllPage();
};

} // TinyRenderer
