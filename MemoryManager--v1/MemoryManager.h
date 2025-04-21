//
// Created by Administrator on 25-3-11.
//
#pragma once
#include <cstddef>
#include "Allocator.h"
#include <cmath>
namespace TinyRenderer {
  /// <summary>
  /// ҳ��Ԫ����СΪ
  /// block_size < sizeof(void*) : block_size * (0xffff-1) �� block_size == 2 ʱΪ131068B ���Ǹ���max_pagesizeΪ2�Ĵη�����Ϊ2��12�η�9182
  /// һ��ֻ����255��ҳ�������ܴ� 9182B * 255 = 2341410B, ��2��22�η� 4GB
  /// ����MemoryManager��С����4GB�ڴ�
  /// </summary>
  class MemoryManager {
    public:
      // �ֽڶ���ֵ
      static unsigned int alignment;
      // allocator���� [1,n]
      static unsigned int allocator_num;
      // ҳ������û��ռ�
      static unsigned int max_userCapacity;
      // �Ƿ����Զ����ڴ����
      static bool isEnable;
      // ���Ƿ���malloc����
      static unsigned char mallocFlag;
    public:
      // ��Applicationͳһ���ó�ʼ��
      void startUp(int alignment, unsigned int max_userCapacity);
      void startUp();
      void shutDown();

      static MemoryManager& instance();

      // ����size + 1�ڴ�飬���ҷ����ض�λ���ڴ�
      virtual void* Allocate(size_t size);
      // �ջ�ptr��ַ���û��ڴ��
      virtual void Deallocate(void* ptr);
      virtual ~MemoryManager() {}

    protected:
      // ����ֵ
      // �ֱ��ʾ�������2��i�η��ڴ��
      Allocator *allocator;
    private:
    void startUp(unsigned int alignment, unsigned int allocator_num, unsigned int max_userCapacity);
      MemoryManager(): allocator(nullptr) {
    } ;
      MemoryManager(const MemoryManager&) = delete;
      MemoryManager& operator=(const MemoryManager&) = delete;
  };

} // TinyRenderer

