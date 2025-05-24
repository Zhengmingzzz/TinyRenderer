//
// Created by Administrator on 25-5-9.
//
#pragma once
#include <string>
#include <rttr/registration>
#include "Function/CommonType/json.h"

namespace TinyRenderer {
    namespace io {
        using namespace std;

        void from_json(const string& string_json, rttr::instance obj);
        void from_json(json& resource_json, rttr::instance obj);
    }// namespace io
}