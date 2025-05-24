//
// Created by Administrator on 25-5-24.
//
#pragma once

#include <vector>

#include "Resource/GUID/GUID.h"


namespace TinyRenderer {
    class Level;

    class LevelConfig {
    public:
        std::vector<GUID> level_guid_array_;
        Level* active_level_ = nullptr;
        std::vector<Level*> level_instance_array_;
    };
}
