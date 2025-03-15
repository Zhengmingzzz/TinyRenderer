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
    // ʣ��block����
    unsigned int block_num;
    // ��ҳblock
    unsigned int block_size;
    Page* next;
public:
    static Page* GetNewPage(unsigned int block_size, unsigned int blockNum_perPage, unsigned int alignment);

    void StartUp(unsigned int blockNum_perPage, unsigned int block_size, void* userMemory);
    void ShutDown();

    void* Allocate();
    void Deallocate(void* rawAddress);
private:
    // �ڴ�����1�ֽ����ڴ洢pageID������pageNum��󲻳���0xff
    static unsigned char pageNum;
    // page��Ψһʶ���
    unsigned char pageID;
    // �û��ռ�����׵�ַ��ƫ����
    void* userMemory;
    // �׸�block��ĵ�ַ��block���������ͣ������void*����
    // ������Page.Deallocate�н���Ϊ�ض����Ͳ�����
    void* freelist;

    // ���ڳ�ʼ��ʱ���ӿ�,block_num��ʾ��1��ʼ�Ŀ�ĸ���,ptr���������ռ���׵�ַ
    static void LinkBlock_Ptr(unsigned int block_num,unsigned int block_size, void* ptr);
    static void LinkBlock_Idx(unsigned int block_num, unsigned int block_size, void* ptr);
};

} // TinyRenderer
