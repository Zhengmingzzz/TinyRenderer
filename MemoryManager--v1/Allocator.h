//
// Created by Administrator on 25-3-11.
//
#pragma once
#include "Page.h"

namespace TinyRenderer {
	class Allocator {
	public:
		// �������������2��twoPowerI�η��ڴ��С
		void startUp(int twoPowerI, unsigned int max_pageSize);
		inline void shutDown()
		{
			FreeAllPage();
		}


		// ���Һ��п��п��page�������ؿ�
		void* Allocate();
		// �����տ��п�ʱ�Ĳ���
		void RecycleBlock();
		void RecyclePage(Page* page);

		// ��ǰAllocator����̶�block_size��С���ڴ�
		unsigned int block_size;
		// ÿ��ҳ��block��������0�����û�п���
		unsigned int blockNum_prePage;
		// �ܹ���ҳ��
		unsigned int page_num_;
		// ���п�����
		unsigned int free_block_num_;
	private:
		Page* pagelist_ = nullptr;
		Page* GetNewPage(unsigned int block_size, unsigned int blockNum_prePage, unsigned int alignment);
		void FreeAllPage();
	};

} // TinyRenderer
