//
// Created by Administrator on 25-3-29.
//
#pragma once

#include "Function/CommonType/json.h"
#include "Core/ThreadPool/TaskResult.h"
#include <iostream>
#include "Platform/Path.h"
#include "Core/StreamSystem/StreamSystem.hpp"


namespace TinyRenderer {
    class ResourceManager
    {
    public:
        static ResourceManager& instance();

        // 同步读T类型文件
        template<typename T>
        T load(Path& path){
            T resource;
            // {
            //     // 上锁
            //     std::lock_guard<std::mutex> lock(mtx);
            //     // 若当前文件并没有存在内存
            //     if (resource_map_.find(path) == resource_map_.end()) {
            //         IStream<T> stream = ResourceManager::GetStream<T>();
            //         if (stream != nullptr) {
            //             resource = stream->template load<T>(path);
            //
            //         }
            //     }
            //
            //     ResourceType rt = resource_map_[path];
            // }
            return resource;
        }

        // 同步写T类型文件
        template<typename T>
        bool SyncSave(Path& path, T resource){
            IStream<T>* stream = ResourceManager::GetStream<T>();
            if (stream != nullptr) {
                return stream->template save<T>(std::move(path), resource);
            }
            return false;
        }


        // 异步读T类型文件
        template<typename T>
        TaskResult<T> async_load(Path&& path){
            IStream<T>* stream = ResourceManager::GetStream<T>();
            if (stream != nullptr) {
                TaskResult<T> tr = stream->async_load(std::move(path));
                return tr;
            }
            std::promise<T> promise;
            std::future<T> future = promise.get_future();
            promise.set_value(T());
            return future;
        }

        // 异步写T类型文件
        template<typename T>
        TaskResult<bool> async_save(Path&& path, T resource){
            IStream<T>* stream = ResourceManager::GetStream<T>();
            if (stream != nullptr) {
                return stream->async_save(std::move(path), resource);
            }
            std::promise<bool> promise;
            std::future<bool> future = promise.get_future();
            promise.set_value(false);
            return future;
        }


        static void AsynIO_CallBack() {
            std::cout << "Call AsynIO_CallBack" << std::endl;
        };


    private:
        static std::mutex mtx;

        // 获取读写T类型文件的Stream，若T未注册则返回nullptr
        template<typename T>
        IStream<T>* GetStream(){
            // 通过返回值类型获取Stream实例
            return StreamType<T>::instance();
        }

        ResourceManager() = default;

        // map是非线程安全类型，需要加锁
        //std::unordered_map<std::string, ResourceType> resource_map_;

    public:
        ResourceManager(ResourceManager const&) = delete;
        ResourceManager& operator=(ResourceManager const&) = delete;
    };
}