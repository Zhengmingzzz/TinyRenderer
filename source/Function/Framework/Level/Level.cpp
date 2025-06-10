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
        auto go_it = root_gameobject_list_.begin();
        while (go_it != root_gameobject_list_.end()) {
            if (*go_it) {
                (*go_it)->unload();
                ++go_it;
            }
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

    Level *Level::create(const std::string &name, const std::filesystem::path &parent_dir) {
        Level* level_ptr = new Level(GUID::allocate_guid(), name);

        AssetManager::get_instance().save_to_meta(parent_dir, level_ptr);
        AssetManager::get_instance().save(level_ptr);

        LevelManager::get_instance().on_create_level(level_ptr);

        return level_ptr;
    }



} // TinyRenderer