//
// Created by Administrator on 25-3-11.
//

#include "Allocator.h"

#include <cmath>

#include "MemoryManager.h"
#include "Function/Message/Message.h"

namespace TinyRenderer {

	void Allocator::startUp(int twoPowerI, unsigned int max_userCapacity) {
		ASSERT(max_userCapacity > 0);
		ASSERT(twoPowerI > 0);

		pagelist_ = nullptr;
		page_num_ = 0;
		free_block_num_ = 0;

		this->block_size = static_cast<unsigned int>(pow(2, twoPowerI));
		// MAX_PAGESIZE��block_size��Ϊ2��i�η���
		blockNum_prePage = block_size >= max_userCapacity ? 1 : max_userCapacity / block_size;
		if (block_size < sizeof(void*))
		{
			// ��������������ܴ��ڵ���0xffff��0xffff���ڱ�ʾ������
			blockNum_prePage = blockNum_prePage >= 0xffff ? 0xffff - 1 : blockNum_prePage;
		}
	}

	void Allocator::FreeAllPage() {
		while (pagelist_ != nullptr) {
			Page* next = pagelist_->next;
			pagelist_->shutDown();
			pagelist_ = next;
		}
	}


	void* Allocator::Allocate() {
		void* result = nullptr;

		// TODO:��̬����2 ������߳�ͬʱʵ����pagelist
		if (pagelist_ == nullptr) {
			pagelist_ = this->GetNewPage(block_size, blockNum_prePage, MemoryManager::alignment);
		}

		// TODO:��̬����3����ʱpage��last���޸�pagelist,pagelistӦ��Ϊԭ�ӱ���
		// ��ǰ��ҳ
		Page* page = pagelist_;
		// ��¼��һ�ڵ�
		Page* last = nullptr;

		while (result == nullptr) {
			// TODO:�˴�Ӧ��������ֹ���߳��ظ�����page����pagelist�ṹ
			// �����ǰpageΪ�գ�˵���Ѿ��ߵ�ĩβҲû�п��еĿ�
			if (page == nullptr) {
				page = this->GetNewPage(block_size, blockNum_prePage, MemoryManager::alignment);
				ASSERT(page != nullptr);

				// �����һ���ڵ����´����Ľڵ�����
				// TODO:�˴����޸�pagelist�����߳�����Ҫע�⣬lastӦ�ñ���ԭ����seq_cst
				last->next = page;

			}
			// �����ǰpage�Ѿ������ˣ�����ת����һ��page
			//TODO: block_numӦ��Ϊԭ�ӵ�
			else if (page->block_num == 0) {
				last = page;
				// TODO: �������̸߳�����pagelist�����ᵼ�·�����Ч�ڴ�
				page = page->next;
			}
			// page��Ҫ����飬���Է����ڴ�
			else {
				// TODO:��̬����4 Allocate��block_num��-1��Ҫ��ֹ��ǰ�̻߳�û-1�����̼߳����������
				result = page->Allocate();
				ASSERT(result != nullptr);
			}

		}
		return result;
	}

	inline Page* Allocator::GetNewPage(unsigned int block_size, unsigned int block_num_per_page, unsigned int alignment) {
		Page* p = Page::GetNewPage(block_size, blockNum_prePage, alignment);
		ASSERT(p != nullptr);
		// ������ҳʱ����¼�ؼ�����
		this->page_num_++;
		this->free_block_num_ += block_num_per_page;
		return p;
	}

	void Allocator::RecycleBlock() {
		this->free_block_num_++;
	}
	void Allocator::RecyclePage(Page* target_page) {
		ASSERT(target_page != nullptr);
		ASSERT(pagelist_ != nullptr);
		bool flag = false;

		if (pagelist_ == target_page) {
			pagelist_->next = target_page->next;
			flag = true;
		}
		else {
			for (Page* p = pagelist_->next, *last_p = pagelist_; p != nullptr; p = p->next) {
				if (p == target_page) {
					this->page_num_--;
					free_block_num_ -= p->block_num;
					last_p->next = p->next;
					flag = true;
					break;
				}
				last_p = p;
			}
		}

		ASSERT(flag == true);
	}


} // TinyRenderer