//
// Created by Administrator on 25-3-30.
//
#pragma once
#include <future>
#include "Function/Message/Message.h"

namespace TinyRenderer {
    template<typename T>
    struct TaskResult {
        TaskResult(){}
        TaskResult(std::future<T>&& fut):future_(std::move(fut)){}
        TaskResult(TaskResult<T>&& other) noexcept:future_(std::move(other.future_)){}

    public:
        bool is_done() {
            if (!future_.valid()) {
                LOG_ERROR("the result future is on_state");
            }
            return future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        }
        // 阻塞的获取返回值
        T get() {
            if (!future_.valid()) {
                LOG_ERROR("the result future is on_state");
            }
            return future_.get();
        }
    public:
        // future不可拷贝
        TaskResult(const TaskResult&) = delete;            // 禁用拷贝构造
        TaskResult& operator=(const TaskResult&) = delete; // 禁用拷贝赋值
    private:
        // 可以用来判断任务是否完成以及返回结果
        std::future<T> future_;
    };
}