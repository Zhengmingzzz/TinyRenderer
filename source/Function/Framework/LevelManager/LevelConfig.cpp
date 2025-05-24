//
// Created by Administrator on 25-5-24.
//

#include "LevelConfig.h"

#include "Resource/GUID/GUID.h"
#include "Function/Framework/Object/Object.h"
#include "Function/Framework/Level/Level.h"

RTTR_REGISTRATION{
    using namespace TinyRenderer;
    rttr::registration::class_<LevelConfig>("level config")
        .property("active level guid", &LevelConfig::active_level_)(
            METADATA_SERIALIZE, METADATA_GUIDTOOBJECT)
        .property("level guid array", &LevelConfig::level_guid_array_)(
            METADATA_SERIALIZE)
        .property("level instance array", &LevelConfig::level_instance_array_)(
            METADATA_SERIALIZE, METADATA_GUIDTOOBJECT);
}