//
// Created by Administrator on 25-5-24.
//
#pragma once

#include <vector>

#include "Resource/GUID/GUID.h"


namespace TinyRenderer{

class LevelConfig {
public:
    std::vector<GUID> level_guid_array_;
    GUID active_level_guid_;
};
}
