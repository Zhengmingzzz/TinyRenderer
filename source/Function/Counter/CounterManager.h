//
// Created by Administrator on 25-4-20.
//

#pragma once
#include "Counter.h"
#include <unordered_map>

namespace TinyRenderer {
    class CounterManager {
    public:
        static CounterManager& get_instance();

        void startUp();
        void shutdown();

        Counter* CreateCounter(const char* name);
        Counter* GetCounter(const char* name);

        ~CounterManager() = default;
    private:
        std::unordered_map<std::string, Counter*> counterMap_;
    public:
        CounterManager(const CounterManager&) = delete;
        CounterManager& operator=(const CounterManager&) = delete;
    private:
        CounterManager() = default;

    };
}

#define Counter_Add(arg_name) \
        do{ \
            Counter* counter_##arg_name = CounterManager::get_instance().GetCounter(#arg_name); \
            if(counter_##arg_name == nullptr){ \
                counter_##arg_name = CounterManager::get_instance().CreateCounter(#arg_name); \
            } \
            counter_##arg_name->fetch_add(); \
        }while(0);

#define Counter_Sub(arg_name) \
        do{ \
            Counter* counter_##arg_name = CounterManager::get_instance().GetCounter(#arg_name); \
            if(counter_##arg_name == nullptr){ \
                counter_##arg_name = CounterManager::get_instance().CreateCounter(#arg_name); \
            } \
            counter_##arg_name->fetch_sub(); \
        }while(0);

#define Counter_Reset(arg_name) \
        do{ \
            Counter* counter_##arg_name = CounterManager::get_instance().GetCounter(#arg_name); \
            if(counter_##arg_name == nullptr){ \
                counter_##arg_name = CounterManager::get_instance().CreateCounter(#arg_name); \
            } \
            counter_##arg_name->reset(); \
        }while(0);

#define Counter_GetCnt(arg_name) \
        do{ \
            Counter* counter_##arg_name = CounterManager::get_instance().GetCounter(#arg_name); \
            if(counter_##arg_name != nullptr){ \
                LOG_INFO("counter__"#arg_name << " sum:" << counter_##arg_name->get() << " times"); \
            } \
            else{ \
                LOG_WARN("counter__"#arg_name << " not exist"); \
            } \
        }while(0);