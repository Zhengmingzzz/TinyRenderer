//
// Created by Administrator on 25-3-11.
//

#include "Allocator.h"

#include <cmath>

#include "MemoryManager.h"
#include "Function/Message/Message.h"

namespace TinyRenderer {

	void Allocator::StartUp(int twoPowerI, unsigned int max_userCapacity) {
		ASSERT(max_userCapacity > 0);
		ASSERT(twoPowerI > 0);
		pagelist = nullptr;

		this->block_size = static_cast<unsigned int>(pow(2, twoPowerI));
		// MAX_PAGESIZE和block_size都为2的i次方，
		blockNum_prePage = block_size >= max_userCapacity ? 1 : max_userCapacity / block_size;
		if (block_size < sizeof(void*))
		{
			// 索引块的数量不能大于等于0xffff，0xffff用于表示空索引
			blockNum_prePage = blockNum_prePage >= 0xffff ? 0xffff - 1 : blockNum_prePage;
		}
	}

	void Allocator::ShutDown() {
		FreeAllPage();
	}

	void Allocator::FreeAllPage()
	{
		while (pagelist != nullptr)
		{
			Page* next = pagelist->next;
			pagelist->ShutDown();
			pagelist = next;
		}
	}


	void* Allocator::Allocate()
	{
		void* result = nullptr;

		if (pagelist == nullptr)
		{
			pagelist = this->GetNewPage(block_size, blockNum_prePage, MemoryManager::alignment);
		}

		// 当前的页
		Page* page = pagelist;
		// 记录上一节点
		Page* last = nullptr;

		while (result == nullptr)
		{
			// 如果当前page为空，说明已经走到末尾也没有空闲的块
			if (page == nullptr)
			{
				page = this->GetNewPage(block_size, blockNum_prePage, MemoryManager::alignment);
				ASSERT(page != nullptr);

				// 将最后一个节点与新创建的节点链接
				last->next = page;

			}
			// 如果当前page已经用完了，则跳转到下一个page
			else if (page->block_num == 0)
			{
				last = page;
				page = page->next;
			}
			else
			{
				result = page->Allocate();
				ASSERT(result != nullptr);
			}

		}
		return result;
	}

	inline Page* Allocator::GetNewPage(unsigned int block_size, unsigned int block_num_per_page, unsigned int alignment) {
		return Page::GetNewPage(block_size, blockNum_prePage, alignment);
	}

} // TinyRenderer