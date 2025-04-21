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
		// MAX_PAGESIZE和block_size都为2的i次方，
		blockNum_prePage = block_size >= max_userCapacity ? 1 : max_userCapacity / block_size;
		if (block_size < sizeof(void*))
		{
			// 索引块的数量不能大于等于0xffff，0xffff用于表示空索引
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

		// TODO:竞态条件2 避免多线程同时实例化pagelist
		if (pagelist_ == nullptr) {
			pagelist_ = this->GetNewPage(block_size, blockNum_prePage, MemoryManager::alignment);
		}

		// TODO:竞态条件3，此时page和last会修改pagelist,pagelist应该为原子变量
		// 当前的页
		Page* page = pagelist_;
		// 记录上一节点
		Page* last = nullptr;

		while (result == nullptr) {
			// TODO:此处应该上锁防止多线程重复创建page更改pagelist结构
			// 如果当前page为空，说明已经走到末尾也没有空闲的块
			if (page == nullptr) {
				page = this->GetNewPage(block_size, blockNum_prePage, MemoryManager::alignment);
				ASSERT(page != nullptr);

				// 将最后一个节点与新创建的节点链接
				// TODO:此处会修改pagelist，多线程下需要注意，last应该保持原子性seq_cst
				last->next = page;

			}
			// 如果当前page已经用完了，则跳转到下一个page
			//TODO: block_num应该为原子的
			else if (page->block_num == 0) {
				last = page;
				// TODO: 若其他线程更改了pagelist，将会导致访问无效内存
				page = page->next;
			}
			// page还要空余块，可以分配内存
			else {
				// TODO:竞态条件4 Allocate后block_num会-1，要防止当前线程还没-1其他线程继续进入分配
				result = page->Allocate();
				ASSERT(result != nullptr);
			}

		}
		return result;
	}

	inline Page* Allocator::GetNewPage(unsigned int block_size, unsigned int block_num_per_page, unsigned int alignment) {
		Page* p = Page::GetNewPage(block_size, blockNum_prePage, alignment);
		ASSERT(p != nullptr);
		// 创建新页时，记录关键参数
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