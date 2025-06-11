//
// Created by Administrator on 25-5-24.
//

#include "Object.h"

#include <glm/vec3.hpp>

REGISTRATION_NO_CONSTRUCTOR_BEGIN(Object)
    .property_readonly("__type__", &Object::get_object_type)(
        METADATA_SERIALIZE)
    .property("name", &Object::name_)(
        METADATA_SERIALIZE)
    .property("is active", &Object::is_active, &Object::set_active)(
        METADATA_SERIALIZE);

// 注册glm类的vec3
    rttr::registration::class_<glm::vec3>("vec3")
    .property("x", &glm::vec3::x)(
        METADATA_SERIALIZE)
    .property("y", &glm::vec3::y)(
        METADATA_SERIALIZE)
    .property("z", &glm::vec3::z)(
        METADATA_SERIALIZE)

REGISTRATION_END

namespace TinyRenderer {
    Object::~Object() {
        // LOG_INFO("object is destroyed!" << " type:" << get_object_type());
    }
} // TinyRenderer