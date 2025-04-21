//
// Created by Administrator on 25-3-28.
//
#pragma once

#include "Core/ThreadPool/TaskResult.h"

namespace TinyRenderer {
    // �������T�����ļ�
    template<class T>
    class IStream {
    public:
        virtual ~IStream() = default;

        // ͬ��IO
        virtual T Load(const char* path) = 0;
        virtual bool Save(const char* path, T resource) = 0;

        // �첽IO
        virtual TaskResult<T> Asyn_Load(const char* path) = 0;
        virtual TaskResult<bool> Asyn_Save(const char* path, T resource) = 0;
    };
}