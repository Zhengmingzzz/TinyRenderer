//
// Created by Administrator on 25-6-5.
//
#pragma once
#include <glm/glm.hpp>

#include "Function/Framework/Component/Component.h"

namespace TinyRenderer {
    class Transform final : public Component {
    public:
        glm::vec3 position_ = {0,0,0};
        glm::vec3 rotation_ = {0,0,0};
        glm::vec3 scale_ = {1,1,1};
    private:
    public:
    private:
        RTTR_ENABLE(Component)
    };
} // TinyRenderer