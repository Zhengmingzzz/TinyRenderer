//
// Created by Administrator on 25-3-11.
//
#pragma once
#include "Page.h"

namespace TinyRenderer {
	class Allocator {
	public:
		// �������������2��twoPowerI�η��ڴ��С
		void StartUp(int twoPowerI, unsigned int max_pageSize);
		inline void ShutDown()
		{
			FreeAllPage();
		}


		// ���Һ��п��п��page�������ؿ�
		void* Allocate();

		// ��ǰAllocator����̶�block_size��С���ڴ�
		unsigned int block_size;
		// ÿ��ҳ��block��������0�����û�п���
		unsigned int blockNum_prePage;
	private:
		Page* pagelist = nullptr;
		Page* GetNewPage(unsigned int block_size, unsigned int blockNum_prePage, unsigned int alignment);
		void FreeAllPage();
	};

} // TinyRenderer
