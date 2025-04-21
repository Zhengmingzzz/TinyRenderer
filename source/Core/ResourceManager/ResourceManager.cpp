//
// Created by Administrator on 25-3-29.
//

#include "ResourceManager.h"
namespace TinyRenderer {
    std::mutex ResourceManager::mtx;

    ResourceManager& ResourceManager::Instance() {
        static ResourceManager* instance = nullptr;
        // 使用二重检测保证不会多次上锁影响效率，又保证线程安全
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(mtx);
            if (instance == nullptr)
                instance = new ResourceManager();
        }
        return *instance;
    }
}