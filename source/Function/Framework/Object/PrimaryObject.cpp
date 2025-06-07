//
// Created by Administrator on 25-6-3.
//

#include "PrimaryObject.h"

REGISTRATION_WITH_CONSTRUCTOR_BEGIN(PrimaryObject)
.property("guid", &PrimaryObject::get_guid, &PrimaryObject::set_guid)(
    METADATA_SERIALIZE)
REGISTRATION_END

namespace TinyRenderer {

} // TinyRenderer