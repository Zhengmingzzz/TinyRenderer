//
// Created by Administrator on 25-3-29.
//
#pragma once
#include "Core/Stream/StreamType.h"
#include "ResourceManager.h"

template<typename T>
    IStream<T>* ResourceManager::GetStream() {
        return TinyRenderer::StreamType<T>::instance();
    }

    template<typename T>
    T ResourceManager::SyncLoad(const char* path) {
        T resource;
        // {
        //     // 上锁
        //     std::lock_guard<std::mutex> lock(mtx);
        //     // 若当前文件并没有存在内存
        //     if (resource_map_.find(path) == resource_map_.end()) {
        //         IStream<T> stream = ResourceManager::GetStream<T>();
        //         if (stream != nullptr) {
        //             resource = stream->template Load<T>(path);
        //
        //         }
        //     }
        //
        //     ResourceType rt = resource_map_[path];
        // }
        return resource;
    }

    template<typename T>
    bool ResourceManager::SyncSave(const char *path, T resource) {
        IStream<T>* stream = ResourceManager::GetStream<T>();
        if (stream != nullptr) {
            return stream->template Save<T>(path, resource);
        }
        return false;
    }

    template<typename T>
    TaskResult<T> ResourceManager::AsynLoad(const char *path) {
        IStream<T>* stream = ResourceManager::GetStream<T>();
        if (stream != nullptr) {
            TaskResult<T> tr = stream->Asyn_Load(path);
            return tr;
        }
        std::promise<T> promise;
        std::future<T> future = promise.get_future();
        promise.set_value(T());
        return future;
    }

    template<typename T>
    TaskResult<bool> ResourceManager::AsynSave(const char* path, T resource) {
        IStream<T>* stream = ResourceManager::GetStream<T>();
        if (stream != nullptr) {
            return stream->Asyn_Save(path, resource);
        }
        std::promise<bool> promise;
        std::future<bool> future = promise.get_future();
        promise.set_value(false);
        return future;
    }
