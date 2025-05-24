//
// Created by Administrator on 25-5-24.
//

#include "Object.h"

RTTR_REGISTRATION{
    using namespace TinyRenderer;
    rttr::registration::class_<TinyRenderer::Object>("Object")
            .constructor<>([]{return new Object();}, rttr::policy::ctor::as_raw_ptr)
            .property("guid", &Object::get_guid, &Object::set_guid)
                        (
                                METADATA_SERIALIZE)
            .property("name", &Object::name_)(
                        METADATA_SERIALIZE)
            .property("is active", &Object::get_active, &Object::set_active)(
                METADATA_SERIALIZE);
};

namespace TinyRenderer {
} // TinyRenderer