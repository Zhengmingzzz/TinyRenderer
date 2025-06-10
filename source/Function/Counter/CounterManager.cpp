//
// Created by Administrator on 25-4-20.
//
#include "CounterManager.h"


#include "Function/Message/Message.h"

namespace TinyRenderer {
    TinyRenderer::CounterManager &TinyRenderer::CounterManager::get_instance() {
        static CounterManager* instance = nullptr;
        if (!instance) {
            instance = new CounterManager();
        }
        return *instance;
    }

    void CounterManager::startUp() {
        get_instance();
    }
    void CounterManager::shutdown() {
        for (auto& counter : counterMap_) {
            delete counter.second;
        }
        counterMap_.clear();
        delete &get_instance();
    }

    Counter* CounterManager::CreateCounter(const char* name) {
        const std::string key(name);
        Counter* counter = nullptr;
        if (counterMap_.find(key) == counterMap_.end()) {
            counter = new Counter();
            counterMap_[key] = counter;
        }
        else
            LOG_WARN(key << " is already existed in CounterMap");
        return counter;
    }

    Counter* CounterManager::GetCounter(const char* name) {
        const std::string key(name);
        Counter* counter = nullptr;
        if (counterMap_.find(key) == counterMap_.end()) {
            counter = CreateCounter(name);
        }
        else {
            counter = counterMap_[key];
        }
        return counter;
    }
}
