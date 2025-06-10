﻿//
// Created by Administrator on 25-5-24.
//

#include "Engine.h"

#include "Core/MemoryManager/MemoryManager.h"
#include "Function/Framework/LevelManager/LevelManager.h"
#include "Function/Framework/ObjectManager/ObjectManager.h"
#include "Platform/ConfigManager/ConfigManager.h"
// #include "Framework/level/level_manager.h"

namespace TinyRenderer {
    Engine& Engine::get_instance()
    {
        static Engine engine_instance;
        return engine_instance;
    }

    void Engine::startup()
    {
        MemoryManager::get_instance().startUp();
        ConfigManager::get_instance().startup();
        LevelManager::get_instance().startup();
    }

    void Engine::shutdown() {
        LevelManager::get_instance().shutdown();
        ObjectManager::get_instance().shutdown();
        MemoryManager::get_instance().shutdown();
    }

    void Engine::run() {
        int i = 1;
        while(i--)
        {
            // 一帧的时间
            float delta_time;
            {
                using namespace std::chrono;
                // 当前帧的时间 - 上一帧的时间 = 间隔时间
                steady_clock::time_point tick_time_point = steady_clock::now();
                duration<float> time_span = duration_cast<duration<float>>(tick_time_point - m_last_tick_time_point);
                delta_time                = time_span.count();

                m_last_tick_time_point = tick_time_point;
            }

            logicalTick(delta_time);
        }
    }

    void Engine::logicalTick(float delta_time)
    {
        LevelManager::get_instance().tick(delta_time);
        ObjectManager::get_instance().tick();
    }
} // TinyRenderer