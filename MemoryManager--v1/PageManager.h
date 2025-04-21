//
// Created by Administrator on 25-3-12.
//
#pragma once
#include <unordered_map>
#include "Page.h"

namespace TinyRenderer {
    class Allocator;

    // 统一管理所有page
    class PageManager {
        public:
            void startUp() {};
            void shutDown();
            PageManager(const PageManager&) = delete;
            PageManager& operator=(const PageManager&) = delete;

        public:
            static PageManager* instance();
            Page* SearchPage(unsigned char pageID);
            void RecordPage(unsigned char pageID, Page* page);
            // 所有page申请内存的接口
            Page* AllocNewPage(unsigned int block_size, unsigned int block_num_per_page, unsigned int alignment);
            // 根据allocator的数据判断是否需要回收
            bool IsRecycle(const Allocator* alloc);
            // 所有page释放内存的接口
            void FreePage(unsigned char pageID);
        private:
            PageManager(){};
            std::unordered_map<unsigned char, Page*> pageMap;
            unsigned short page_num_ = 0;
    };

} // TinyRenderer

