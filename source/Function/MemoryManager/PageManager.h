//
// Created by Administrator on 25-3-12.
//
#pragma once
#include <unordered_map>
#include "Page.h"

namespace TinyRenderer {
    // ͳһ��������page
    class PageManager {
        public:
            void StartUp() {};
            void ShutDown();
            PageManager(const PageManager&) = delete;
            PageManager& operator=(const PageManager&) = delete;

        public:
            static PageManager* GetInstance();
            Page* SearchPage(unsigned char pageID);
            void RecordPage(unsigned char pageID, Page* page);
            // ����page�����ڴ�Ľӿ�
            Page* AllocNewPage(unsigned int block_size, unsigned int block_num_per_page, unsigned int alignment);
            // ����page�ͷ��ڴ�Ľӿ�
            void FreePage(unsigned char pageID);
        private:
          PageManager(){};
          std::unordered_map<unsigned char, Page*> pageMap;
    };

} // TinyRenderer

