//
// Created by Administrator on 25-5-24.
//

#include "Component.h"
#include "Function/Framework/GameObject/GameObject.h"

REGISTRATION_NO_CONSTRUCTOR_BEGIN(Component)
    .property("owner_object", &Component::get_owner_object, &Component::set_owner_object)(
        METADATA_SERIALIZE, METADATA_GUIDTOOBJECT);

    // TODO:map中的反序列化时，需要在此创建
    rttr::registration::class_<std::vector<Component*>>("components vector")
        .constructor()(rttr::policy::ctor::as_object);
    rttr::registration::class_<std::list<Component*>>("components list")
        .constructor()(rttr::policy::ctor::as_object);
REGISTRATION_END

namespace TinyRenderer {
    Component::~Component() {}


    void Component::set_owner_object(GameObject *parent) {
        if (parent)
            owner_object_ = parent;
    }

    GameObject* Component::get_owner_object() const {
        return owner_object_;
    }

} // TinyRenderer