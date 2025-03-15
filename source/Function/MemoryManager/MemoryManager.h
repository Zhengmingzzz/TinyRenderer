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
      // 块的上一字节的标记
      static unsigned char mallocFlag;
    public:
      // 由Application统一调用初始化
      void StartUp(unsigned int alignment, unsigned int max_userCapacity);
      void StartUp();
      void ShutDown();

      static MemoryManager& GetInstance();

      // 分配size + 1内存块，并且返回特定位置内存
      virtual void* Allocate(size_t size);
      // 收回ptr地址的用户内存块?
      virtual void Deallocate(void* ptr);
      virtual ~MemoryManager() {}

    protected:
      // 对齐值
      // 分别表示负责分配2的i次方内存块
      Allocator *allocator;
    private:
    void StartUp(unsigned int alignment, unsigned int allocator_num, unsigned int max_userCapacity);
      MemoryManager(): allocator(nullptr) {
    } ;
      MemoryManager(const MemoryManager&) = delete;
      MemoryManager& operator=(const MemoryManager&) = delete;

  };

} // TinyRenderer

