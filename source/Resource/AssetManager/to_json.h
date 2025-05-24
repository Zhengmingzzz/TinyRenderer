//
// Created by Administrator on 25-5-7.
//
#pragma once

#include "Function/CommonType/json.h"
#include <rttr/registration>
namespace TinyRenderer {
    namespace io
    {
        ordered_json to_json(rttr::instance obj);
    }
}
