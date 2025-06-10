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
        void awake() override {
            std::cout << "Awake" << std::endl;
        }
    public:

    private:

        RTTR_ENABLE(Component)
    };
} // TinyRenderer