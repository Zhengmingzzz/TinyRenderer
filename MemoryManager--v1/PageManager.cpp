//
// Created by Administrator on 25-3-12.
//

#include "PageManager.h"
#include "Function/Message/Message.h"
#include "MemoryManager.h"
#include "Allocator.h"

namespace TinyRenderer 
{
	
	void PageManager::shutDown()
	{
		if (pageMap.size() > 0)
		{
			LOG_ERROR("page still exist");
		}
		delete instance();
	}

	PageManager* PageManager::instance()
	{
		static PageManager* instance = nullptr;
		if (instance == nullptr)
		{
			MemoryManager::isEnable = false;
			instance = new PageManager();
			MemoryManager::isEnable = true;
		}
		return instance;
	}
	Page* PageManager::SearchPage(unsigned char pageID) 
	{
		return pageMap[pageID];
	}
	void PageManager::RecordPage(unsigned char pageID, Page* page) 
	{
		if (pageMap.find(pageID) == pageMap.end()) 
		{
			MemoryManager::isEnable = false;
			pageMap[pageID] = page;
			MemoryManager::isEnable = true;
		}
		else 
		{
			LOG_ERROR("Page already exists :" + pageID);
		}
	}

	Page* PageManager::AllocNewPage(unsigned int block_size, unsigned int block_num_per_page, unsigned int alignment)
	{
		ASSERT(alignment % 2 == 0)
		// 直接使用初始对齐值
		//alignment = alignment > block_size ? block_size : alignment;
		size_t extension_size = block_num_per_page * block_size + alignment + sizeof(Page);

		void* ptr = malloc(extension_size);

		ASSERT(ptr != nullptr);

		// 将这块内存第一部分解释成Page类型
		Page* new_page = reinterpret_cast<Page*>(ptr);
		// 计算出用户空间对齐后的对齐地址
		uintptr_t rawAddress = reinterpret_cast<uintptr_t>(ptr) + sizeof(Page);
		unsigned int mask = alignment - 1;
		unsigned int misalignment = rawAddress & mask;
		unsigned int offset = alignment - misalignment;
		void* alignedAddress = reinterpret_cast<void*>(rawAddress + offset);

		new_page->startUp(block_num_per_page, block_size, alignedAddress);
		// 获取pageID后需要向PageManager注册，方便查找
		PageManager::instance()->RecordPage(new_page->pageID, new_page);
		return new_page;
	}

	void PageManager::FreePage(unsigned char pageID) 
	{
		if (pageMap.find(pageID) == pageMap.end()) 
		{
			LOG_ERROR("Page not found :" + pageID);
		}
		Page* page = pageMap[pageID];
		pageMap.erase(pageID);

		// 如果当前页还要引用就要求释放则报错
		if (page->block_num != page->max_block_num) {
			LOG_ERROR("the page:" << (int)pageID << " memory leak");
		}

		free(page);

	}

	bool PageManager::IsRecycle(const Allocator* alloc) {
		return false;
	}
} // TinyRenderer