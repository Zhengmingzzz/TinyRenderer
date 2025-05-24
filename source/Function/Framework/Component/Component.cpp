//
// Created by Administrator on 25-5-24.
//

#include "Component.h"
#include "Function/Framework/GameObject/GameObject.h"

RTTR_REGISTRATION {
    using namespace TinyRenderer;
    rttr::registration::class_<Component>("Component")
    .property("owner_object", &Component::get_owner_object, &Component::set_owner_object)(
        METADATA_SERIALIZE, METADATA_GUIDTOOBJECT);
}

namespace TinyRenderer {

    void Component::set_owner_object(GameObject *parent) {
        if (parent)
            owner_object_ = parent;
    }

    GameObject* Component::get_owner_object() const {
        return owner_object_;;
    }

} // TinyRenderer