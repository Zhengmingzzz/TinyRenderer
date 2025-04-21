//
// Created by Administrator on 25-3-11.
//
#pragma once
#include "Page.h"

namespace TinyRenderer {
	class Allocator {
	public:
		// 这个分配器分配2的twoPowerI次方内存大小
		void startUp(int twoPowerI, unsigned int max_pageSize);
		inline void shutDown()
		{
			FreeAllPage();
		}


		// 查找含有空闲块的page，并返回块
		void* Allocate();
		// 当回收空闲块时的操作
		void RecycleBlock();
		void RecyclePage(Page* page);

		// 当前Allocator分配固定block_size大小的内存
		unsigned int block_size;
		// 每个页的block数量，到0则代表没有块了
		unsigned int blockNum_prePage;
		// 总共的页数
		unsigned int page_num_;
		// 空闲块数量
		unsigned int free_block_num_;
	private:
		Page* pagelist_ = nullptr;
		Page* GetNewPage(unsigned int block_size, unsigned int blockNum_prePage, unsigned int alignment);
		void FreeAllPage();
	};

} // TinyRenderer
