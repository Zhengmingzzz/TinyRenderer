//
// Created by Administrator on 25-5-24.
//

#include "Level.h"

#include "Function/Framework/ObjectManager/ObjectManager.h"
#include "Function/Framework/GameObject/GameObject.h"
#include "Resource/AssetManager/AssetManager.h"

REGISTRATION_WITH_CONSTRUCTOR_BEGIN(Level)
        .property("gameobject instance array", &Level::root_gameobject_list_)(
            METADATA_SERIALIZE, METADATA_GUIDTOOBJECT);
REGISTRATION_END

namespace TinyRenderer {

    void Level::save() {
        AssetManager::get_instance().save(this);
        for (auto& go_ptr : root_gameobject_list_) {
            if (go_ptr)
                go_ptr->save();
        }
    }


    void Level::unload() {
        if (LevelManager::get_instance().on_unload_level(get_guid()) == false)
            return;

        set_active(false);

        for (auto& go_ptr : root_gameobject_list_) {
            // if (go_ptr)
            //     go_ptr->unload();
        }
        // 由object_manager统一销毁
        ObjectManager::get_instance().unload_object(this);
    }

    void Level::on_add_child(HierarchyNode* node) {
        if (auto go = rttr::rttr_cast<GameObject*>(node)) {
            root_gameobject_list_.push_back(go);
        }
    }

    void Level::on_remove_child(HierarchyNode* node) {
        if (auto go = rttr::rttr_cast<GameObject*>(node)) {
            root_gameobject_list_.remove(go);
        }
    }


} // TinyRenderer