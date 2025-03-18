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
		// MAX_PAGESIZE��block_size��Ϊ2��i�η���
		blockNum_prePage = block_size >= max_userCapacity ? 1 : max_userCapacity / block_size;
		if (block_size < sizeof(void*))
		{
			// ��������������ܴ��ڵ���0xffff��0xffff���ڱ�ʾ������
			blockNum_prePage = blockNum_prePage >= 0xffff ? 0xffff - 1 : blockNum_prePage;
		}
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

		// ��ǰ��ҳ
		Page* page = pagelist;
		// ��¼��һ�ڵ�
		Page* last = nullptr;

		while (result == nullptr)
		{
			// �����ǰpageΪ�գ�˵���Ѿ��ߵ�ĩβҲû�п��еĿ�
			if (page == nullptr)
			{
				page = this->GetNewPage(block_size, blockNum_prePage, MemoryManager::alignment);
				ASSERT(page != nullptr);

				// �����һ���ڵ����´����Ľڵ�����
				last->next = page;

			}
			// �����ǰpage�Ѿ������ˣ�����ת����һ��page
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