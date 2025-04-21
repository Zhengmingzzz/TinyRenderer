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
        // �����Ļ�ȡ����ֵ
        T get() {
            if (!future_.valid()) {
                LOG_ERROR("the result future is on_state");
            }
            return future_.get();
        }
    public:
        // future���ɿ���
        TaskResult(const TaskResult&) = delete;            // ���ÿ�������
        TaskResult& operator=(const TaskResult&) = delete; // ���ÿ�����ֵ
    private:
        // ���������ж������Ƿ�����Լ����ؽ��
        std::future<T> future_;
    };
}