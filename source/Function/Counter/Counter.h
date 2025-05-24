//
// Created by Administrator on 25-4-20.
//
#pragma once
#include <atomic>
#include <string>

namespace TinyRenderer {
    // 计数器类，负责计数
    class Counter {
      public:
        Counter():counter_(0){}
        void fetch_add() {
            counter_.fetch_add(1, std::memory_order_relaxed);
        }
        void reset() {
            counter_.exchange(0, std::memory_order_relaxed);
        }
        void fetch_sub() {
            counter_.fetch_sub(1, std::memory_order_relaxed);
        }

        size_t get() {
            return counter_.load(std::memory_order_relaxed);
        }

      private:
        std::atomic<size_t> counter_;
    };

} // TinyRenderer