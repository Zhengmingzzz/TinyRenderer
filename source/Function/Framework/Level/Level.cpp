//
// Created by Administrator on 25-5-24.
//

#include "Level.h"

#include "Function/Framework/ObjectManager/ObjectManager.h"
#include "Function/Framework/GameObject/GameObject.h"

RTTR_REGISTRATION {
    using namespace TinyRenderer;
    rttr::registration::class_<Level>("Level")
        .constructor([]{return new Level();} ,rttr::policy::ctor::as_raw_ptr)
        .property("gameobject instance array", &Level::gameobject_instance_array_);
}

namespace TinyRenderer {


    void Level::save() {
        // AssetManager::get_instance().save(this);
        for (auto& go_ptr : gameobject_instance_array_) {
            // if (go_ptr)
            //     go_ptr->save();
        }
    }

    void Level::tick() {
        for (auto& go_ptr : gameobject_instance_array_) {
            // if (go_ptr && go_ptr->is_active_)
            //     go_ptr->tick();
        }
    }

    void Level::unload() {
        for (auto& go_ptr : gameobject_instance_array_) {
            // if (go_ptr)
            //     go_ptr->unload();
        }
        // 由object_manager统一销毁
        ObjectManager::get_instance().unload_object(this);
    }

} // TinyRenderer