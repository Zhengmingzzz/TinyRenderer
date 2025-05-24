//
// Created by Administrator on 25-5-15.
//

#include "GUID.h"

#include <rttr/registration>

    RTTR_REGISTRATION{
        rttr::registration::class_<TinyRenderer::GUID>("GUID")
                .property("guid", &TinyRenderer::GUID::guid_);
    };