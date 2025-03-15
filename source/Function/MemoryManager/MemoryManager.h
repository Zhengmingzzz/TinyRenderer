//
// Created by Administrator on 25-3-11.
//
#pragma once
#include <cstddef>
#include "Allocator.h"
namespace TinyRenderer {
  class MemoryManager {
    public:
      static unsigned int alignment;
      static unsigned int allocator_num;
      static unsigned int max_userCapacity;
      static bool isEnable;
      // �����һ�ֽڵı��
      static unsigned char mallocFlag;
    public:
      // ��Applicationͳһ���ó�ʼ��
      void StartUp(unsigned int alignment, unsigned int max_userCapacity);
      void StartUp();
      void ShutDown();

      static MemoryManager& GetInstance();

      // ����size + 1�ڴ�飬���ҷ����ض�λ���ڴ�
      virtual void* Allocate(size_t size);
      // �ջ�ptr��ַ���û��ڴ��?
      virtual void Deallocate(void* ptr);
      virtual ~MemoryManager() {}

    protected:
      // ����ֵ
      // �ֱ��ʾ�������2��i�η��ڴ��
      Allocator *allocator;
    private:
    void StartUp(unsigned int alignment, unsigned int allocator_num, unsigned int max_userCapacity);
      MemoryManager(): allocator(nullptr) {
    } ;
      MemoryManager(const MemoryManager&) = delete;
      MemoryManager& operator=(const MemoryManager&) = delete;

  };

} // TinyRenderer

