//
// Created by Administrator on 25-3-12.
//
#pragma once
#include <unordered_map>
#include "Page.h"

namespace TinyRenderer {
    // 统一管理所有page
    class PageManager {
        public:
            PageManager(const PageManager&) = delete;
            PageManager& operator=(const PageManager&) = delete;
        public:
            static PageManager& GetInstance();
            Page* SearchPage(unsigned char pageID);
            void RecordPage(unsigned char pageID, Page* page);
            Page* AllocNewPage(unsigned int block_size, unsigned int block_num_per_page, unsigned int alignment);
            void FreePage(unsigned char pageID);
        public:



        // 表示如果用malloc申请的空间的标识，如果标志 = mallocFlag，直接用free
        static const unsigned char mallocFlag;
        private:
          static std::unordered_map<unsigned char, Page*> pageMap;
          PageManager(){};


    };

} // TinyRenderer

