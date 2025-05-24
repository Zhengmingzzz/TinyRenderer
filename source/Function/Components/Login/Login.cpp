//
// Created by Administrator on 25-5-27.
//

#include "Login.h"



REGISTRATION_WITH_CONSTRUCTOR_BEGIN(Login)
    .property("position", &Login::position_);

rttr::registration::class_<glm::vec3>("vec3")
    .property("x", &glm::vec3::x)
    .property("y", &glm::vec3::y)
    .property("z", &glm::vec3::z);
REGISTRATION_END


namespace TinyRenderer {
} // TinyRenderer