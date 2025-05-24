//
// Created by Administrator on 25-5-15.
//

#include "GUID.h"

#include <rttr/registration>

#include "Function/Framework/Object/Object.h"

RTTR_REGISTRATION {
        rttr::registration::class_<TinyRenderer::GUID>("GUID")
                .property("guid", &TinyRenderer::GUID::guid_)(
                    METADATA_SERIALIZE);
    };

