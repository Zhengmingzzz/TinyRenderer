//
// Created by Administrator on 25-3-12.
//
#pragma once
#include "Function/Message/Message.h"
namespace TinyRenderer
{
	// 用于管理链接比指针类型小的块,索引从0开始
	struct Idx_block
	{
		// 下一个块所在的索引,索引范围为[0,65535],0xffff块不允许分配，表示空索引
		unsigned short next_block;

		// （当前地址 - 将用户空间首地址）/ block_size 得到从0开始的索引
		// 如果传入一个空地址，则返回0xffff
		static unsigned short GetIdx(void* cur_address, void* userMemory_address, unsigned int block_size)
		{
			ASSERT(userMemory_address != nullptr);
			unsigned short res = 0xffff;
			if (cur_address != nullptr)
			{
				unsigned char* uc_cur_address = reinterpret_cast<unsigned char*>(cur_address);
				unsigned char* uc_userMemory_address = reinterpret_cast<unsigned char*>(userMemory_address);
				res = (uc_cur_address - uc_userMemory_address) / block_size;
			}

			return res;
		}

		// idx从0开始，公式为(idx * block_size) + userMemoeyAddress
		static Idx_block* GetAddress(unsigned short idx, void* userMemory_address, unsigned int block_size)
		{
			ASSERT(idx >= 0 && idx <= 0xffff);
			ASSERT(block_size >= 2);
			ASSERT(userMemory_address != nullptr);
			Idx_block* res = nullptr;
			unsigned char* uc_userMemory_address = nullptr;

			if (idx != 0xffff)
				uc_userMemory_address = reinterpret_cast<unsigned char*>(userMemory_address);
			return reinterpret_cast<Idx_block*>((idx * block_size) + uc_userMemory_address);
		}
	};

	// 管理>=指针类型的块
	struct Ptr_block {
		Ptr_block* next;
	};
}