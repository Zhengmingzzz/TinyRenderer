//
// Created by Administrator on 25-3-11.
//
#pragma once
#include "Page.h"

namespace TinyRenderer {
class Allocator {
public:
    // �������������2��twoPowerI�η��ڴ��С
    void StartUp(int twoPowerI, unsigned int max_pageSize);
    void ShutDown();

    // ֻ���𷵻��ص��С���ڴ�飬������Ĳ���
    void* Allocate();

    // ��ǰAllocator����̶�block_size��С���ڴ�
    unsigned int block_size;
    // ÿ��ҳ��block����
    unsigned int blockNum_prePage;
private:
    Page* pagelist = nullptr;
    Page* GetNewPage(unsigned int block_size, unsigned int blockNum_prePage, unsigned int alignment);
    void FreeAllPage();
};

} // TinyRenderer
