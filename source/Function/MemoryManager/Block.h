//
// Created by Administrator on 25-3-12.
//
#pragma once
#include "Function/Message/Message.h"
// ���ڹ������ӱ�ָ������С�Ŀ�,������0��ʼ
struct Idx_block {
	// ��һ�������ڵ�����,������ΧΪ[0,65535],0xffff�鲻������䣬��ʾ������
	unsigned short next_block;

	// ����ǰ��ַ - ���û��ռ��׵�ַ��/ block_size �õ���0��ʼ������
	static unsigned short GetIdx(void* cur_address, void* userMemory_address, unsigned int block_size)
	{
		unsigned char* uc_cur_address = reinterpret_cast<unsigned char*>(cur_address);
		unsigned char* uc_userMemory_address = reinterpret_cast<unsigned char*>(userMemory_address);

		return (uc_cur_address - uc_userMemory_address) / block_size;
	}

	// idx��0��ʼ����ʽΪ(idx * block_size) + userMemoeyAddress
	static Idx_block* GetAddress(unsigned short idx, void* userMemory_address, unsigned int block_size)
	{
		ASSERT(idx >= 0 && idx < 0xffff)
		unsigned char* uc_userMemory_address = reinterpret_cast<unsigned char*>(userMemory_address);
		return reinterpret_cast<Idx_block*>((idx * block_size) + uc_userMemory_address);
	}
};

// ����>=ָ�����͵Ŀ�
struct Ptr_block {
	Ptr_block* next;
};