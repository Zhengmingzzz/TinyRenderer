//
// Created by Administrator on 25-5-24.
//

#include "Component.h"
#include "Function/Framework/GameObject/GameObject.h"

REGISTRATION_NO_CONSTRUCTOR_BEGIN(Component)
    .property("owner_object", &Component::get_owner_object, &Component::set_owner_object)(
        METADATA_SERIALIZE, METADATA_GUIDTOOBJECT);

    rttr::registration::class_<std::vector<Component*>>("components vector")
        .constructor()(rttr::policy::ctor::as_object);
    rttr::registration::class_<std::list<Component*>>("components list")
        .constructor()(rttr::policy::ctor::as_object);
REGISTRATION_END

namespace TinyRenderer {

    Component::~Component() {
        if (owner_object_ && owner_object_->is_destroyed_ == false) {
            owner_object_->on_unload_component(this);
        }
        owner_object_ = nullptr;
    }

    void Component::set_owner_object(GameObject *parent) {
        if (parent)
            owner_object_ = parent;
    }

    GameObject* Component::get_owner_object() const {
        return owner_object_;
    }

} // TinyRenderer