//
// Created by Administrator on 25-5-24.
//
#pragma once
#include "Function/Framework/LevelManager/LevelManager.h"
#include "Function/Framework/GameObject/GameObject.h"
#include "Function/Framework/HierarchyNode/HierarchyNode.h"

namespace TinyRenderer {
    class GameObject;

    class Level : public HierarchyNode {
    public:
        std::list<GameObject*> root_gameobject_list_; // 只存储根GO，GO下的子GO由它的父GO管理
    private:

    public:
        Level() = default;
        Level(const GUID& guid) : HierarchyNode(guid) {}
        Level(const GUID& guid, const std::string& name) : HierarchyNode(guid, name) {}
        ~Level() override {
            LevelManager::get_instance().on_unload_level(this);
        }

        static Level* create(const std::string& name = "default level", const std::filesystem::path &parent_dir = ConfigManager::get_instance().get_asset_fodder_path()/"Level");
        GameObject* get_gameobject(const std::string& name);

        void save();
        void tick();

        void unload();

    private:
        friend void GameObject::set_parent(HierarchyNode *parent);
        void on_add_child(HierarchyNode* node) override;
        void on_remove_child(HierarchyNode* node) override;

        RTTR_ENABLE(HierarchyNode)
    };
} // TinyRenderer

