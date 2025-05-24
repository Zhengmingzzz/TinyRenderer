//
// Created by Administrator on 25-5-24.
//
#pragma once

#include <chrono>

namespace TinyRenderer {

class Engine {
public:
    std::chrono::steady_clock::time_point m_last_tick_time_point {std::chrono::steady_clock::now()};

private:
public:
    static Engine& get_instance();
    void startup();
    void shutdown();
    void run();
    void logicalTick(float delta_time);
    bool rendererTick();

public:
};

} // TinyRenderer
