//
// Created by Administrator on 25-3-29.
//

#include "ResourceManager.h"
namespace TinyRenderer {
    std::mutex ResourceManager::mtx;

    ResourceManager& ResourceManager::Instance() {
        static ResourceManager* instance = nullptr;
        // ʹ�ö��ؼ�Ᵽ֤����������Ӱ��Ч�ʣ��ֱ�֤�̰߳�ȫ
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(mtx);
            if (instance == nullptr)
                instance = new ResourceManager();
        }
        return *instance;
    }
}