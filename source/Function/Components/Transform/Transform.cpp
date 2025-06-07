//
// Created by Administrator on 25-6-5.
//

#include "Transform.h"

REGISTRATION_WITH_CONSTRUCTOR_BEGIN(Transform)
.property("position", &Transform::position_)(
    METADATA_SERIALIZE)
.property("rotation", &Transform::rotation_)(
    METADATA_SERIALIZE)
.property("scale", &Transform::scale_)(
    METADATA_SERIALIZE)
REGISTRATION_END

namespace TinyRenderer {
} // TinyRenderer