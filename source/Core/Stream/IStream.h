//
// Created by Administrator on 25-3-28.
//
#pragma once

#include "Core/ThreadPool/TaskResult.h"

namespace TinyRenderer {
    // 负责操作T类型文件
    template<class T>
    class IStream {
    public:
        virtual ~IStream() = default;

        // 同步IO
        virtual T Load(const char* path) = 0;
        virtual bool Save(const char* path, T resource) = 0;

        // 异步IO
        virtual TaskResult<T> Asyn_Load(const char* path) = 0;
        virtual TaskResult<bool> Asyn_Save(const char* path, T resource) = 0;
    };
}