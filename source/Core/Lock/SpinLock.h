//
// Created by Administrator on 25-4-16.
//
#pragma once

#include <atomic>
#include <mutex>

class SpinLock {
public:
    SpinLock() = default;
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;

    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {}
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }

public:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
};
