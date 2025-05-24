//
// Created by Administrator on 25-5-24.
//

#include "GUIDReference.h"
#include "Function/Framework/Object/Object.h"

namespace TinyRenderer {
    void GUIDReference::register_object(const GUID& object_guid, Object *object_ptr) {
        if(!object_instance_map_.contains(object_guid) && object_guid.is_valid() && object_ptr != nullptr){
            object_instance_map_[object_guid] = object_ptr;
        }
        else{
            std::cerr << "GUIDReference registe object error" << std::endl;
        }
    }
} // TinyRenderer