//
// Created by Administrator on 25-3-12.
//

#include "PageManager.h"
#include "Function/Message/Message.h"
#include "MemoryManager.h"

namespace TinyRenderer {
	const unsigned char PageManager::mallocFlag = 0xff;
	std::unordered_map<unsigned char, Page*> PageManager::pageMap;

	PageManager& PageManager::GetInstance() {
		static PageManager instance;
		return instance;
	}
	Page* PageManager::SearchPage(unsigned char pageID) {
		return pageMap[pageID];
	}
	void PageManager::RecordPage(unsigned char pageID, Page* page) {
		if (pageMap.find(pageID) == pageMap.end()) {
			MemoryManager::isEnable = false;
			pageMap[pageID] = page;
			MemoryManager::isEnable = true;
		}
		else {
			LOG_ERROR("Page already exists :" + pageID);
		}
	}

	Page* PageManager::AllocNewPage(unsigned int block_size, unsigned int block_num_per_page, unsigned int alignment) {
		ASSERT(alignment % 2 == 0)
		// ���ö���ֵΪ��С��һ��
		alignment = alignment > block_size ? block_size : alignment;
		size_t extension_size = block_num_per_page * block_size + alignment + sizeof(Page);

		void* ptr = malloc(extension_size);
		ASSERT(ptr != nullptr);

		// ������ڴ��һ���ֽ��ͳ�Page����
		Page* new_page = reinterpret_cast<Page*>(ptr);
		// ������û��ռ�����Ķ����ַ
		uintptr_t rawAddress = reinterpret_cast<uintptr_t>(ptr) + sizeof(Page);
		unsigned int mask = alignment - 1;
		unsigned int misalignment = rawAddress & mask;
		unsigned int offset = alignment - misalignment;
		void* alignedAddress = reinterpret_cast<void*>(rawAddress + offset);

		new_page->StartUp(block_num_per_page, block_size, alignedAddress);
		// ��ȡpageID����Ҫ��PageManagerע�ᣬ�������
		PageManager::GetInstance().RecordPage(new_page->pageID, new_page);
		return new_page;
	}

	void PageManager::FreePage(unsigned char pageID) {
		if (pageMap.find(pageID) == pageMap.end()) {
			LOG_ERROR("Page not found :" + pageID);
		}
		Page* page = pageMap[pageID];
		pageMap.erase(pageID);

		if (page != NULL) {
			free(page);
		}
	}

} // TinyRenderer