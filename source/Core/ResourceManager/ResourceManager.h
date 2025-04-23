//
// Created by Administrator on 25-3-29.
//
#pragma once

#include "Function/CommonType/json.h"
#include "Core/Stream/IStream.h"
#include "Core/ThreadPool/TaskResult.h"
#include <iostream>

namespace TinyRenderer {
    class ResourceManager
    {
    public:
        static ResourceManager& Instance();

        // 同步读T类型文件
        template<typename T>
        T SyncLoad(const char* path);

        // 同步写T类型文件
        template<typename T>
        bool SyncSave(const char* path, T resource);

        // 异步读T类型文件
        template<typename T>
        TaskResult<T> AsynLoad(const char* path);

        // 异步写T类型文件
        template<typename T>
        TaskResult<bool> AsynSave(const char* path, T resource);


        static void AsynIO_CallBack() {
            std::cout << "Call AsynIO_CallBack" << std::endl;
        };


    private:
        // 获取读写T类型文件的Stream，若T未注册则返回nullptr
        template<typename T>
        IStream<T>* GetStream();

        static std::mutex mtx;
        ResourceManager(){};

        // map是非线程安全类型，需要加锁
        //std::unordered_map<std::string, ResourceType> resource_map_;

    public:
        ResourceManager(ResourceManager const&) = delete;
        ResourceManager& operator=(ResourceManager const&) = delete;
    };
#include "ResourceManager.inl"
}