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
        std::vector<Level*> level_instance_array_;
    };
}
