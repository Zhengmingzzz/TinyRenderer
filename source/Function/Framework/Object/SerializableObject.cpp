//
// Created by Administrator on 25-6-3.
//

#include "SerializableObject.h"

REGISTRATION_WITH_CONSTRUCTOR_BEGIN(SerializableObject)
.property("guid", &SerializableObject::get_guid, &SerializableObject::set_guid)(
    METADATA_SERIALIZE)
REGISTRATION_END

namespace TinyRenderer {

} // TinyRenderer