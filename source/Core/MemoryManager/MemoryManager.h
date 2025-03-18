//
// Created by Administrator on 25-3-11.
//
#pragma once
#include <cstddef>
#include "Allocator.h"
#include <cmath>
namespace TinyRenderer {
  /// <summary>
  /// 页内元素最小为
  /// block_size < sizeof(void*) : block_size * (0xffff-1) 即 block_size == 2 时为131068B 但是根据max_pagesize为2的次方限制为2的12次方9182
  /// 一共只能有255个页，最少能存 9182 * 255 = 2341410B, 即2的22次方 4GB
  /// 所以MemoryManager最小能有4GB内存
  /// </summary>
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
        inline void StartUp(unsigned int alignment, unsigned int max_userCapacity) {
            // 计算出最多能有多少个allocator，最后一个allocator只能分配一个块，直接用malloc就行
            this->allocator_num = std::log(max_userCapacity) / log(2) - 1;
            this->StartUp(alignment, this->allocator_num, max_userCapacity);
        }
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

