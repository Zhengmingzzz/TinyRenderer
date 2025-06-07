//
// Created by Administrator on 25-6-6.
//

#include "login.h"

#include "Function/Framework/Object/Object.h"

REGISTRATION_WITH_CONSTRUCTOR_BEGIN(login)
.property("example_str", &login::example_str)(
    METADATA_SERIALIZE)
REGISTRATION_END

namespace TinyRenderer {
} // TinyRenderer