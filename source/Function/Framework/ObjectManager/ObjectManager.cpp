//
// Created by Administrator on 25-5-24.
//

#include "ObjectManager.h"

#include "Core/GUIDReference/GUIDReference.h"
#include "Function/Framework/Object/Object.h"

namespace TinyRenderer {
    void ObjectManager::unload_object(const GUID &object_guid) {
        Object* obj_ptr = GUIDReference::get_instance().get_object(object_guid);
        if (object_guid.is_valid() && obj_ptr) {
            GUIDReference::get_instance().unregister_object(object_guid);
            unload_object(obj_ptr);
        }
    }

    void ObjectManager::unload_object(Object *object) {
        // unload_object_array_.push_back(object);
        delete object;
    }
} // TinyRenderer