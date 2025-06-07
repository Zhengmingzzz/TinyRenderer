//
// Created by Administrator on 25-6-6.
//
#pragma once
#include <string>

#include "Function/Framework/Component/Component.h"

namespace TinyRenderer {
    class login final : public Component {
    public:
        std::string example_str;
    private:
    public:
    private:

        RTTR_ENABLE(Component)
    };
} // TinyRenderer