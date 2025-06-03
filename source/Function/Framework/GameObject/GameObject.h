//
// Created by Administrator on 25-5-24.
//
#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <rttr/registration>

#include "Function/Framework/Component/Component.h"
#include "Function/Framework/LevelManager/LevelManager.h"
#include "Function/Framework/Object/HierarchyNode.h"
#include "Function/Framework/Object/SerializableObject.h"

namespace TinyRenderer {
    class Level;

    class GameObject : public HierarchyNode, public SerializableObject {
    public:
        std::list<GameObject*> children_list_;

    private:
        HierarchyNode* parent_node_ = nullptr; // 有可能是GO，也有可能是Level，表示上一层级的节点

        std::unordered_map<std::string, std::vector<Component*>> component_map_;

    public:
        // 默认属于
        static GameObject* create(const std::string& name = "default GO", GameObject* parent_ = nullptr ,const GUID& guid = GUID::allocate_guid());
        GameObject()  = default;
        GameObject(const GUID& guid, const std::string& name) : SerializableObject(guid, name) {
        }

        template<class T>
        T* add_component() {
            rttr::type t = rttr::type::get<T>();
            // 必须继承自component
            if (!t.is_derived_from(rttr::type::get<Component>()))
                return nullptr;

            rttr::variant var = t.create();
            if (!var.is_valid()) {
                var.get_type().destroy(var);
            }

            Component* com = var.get_value<Component*>();
            std::string map_key = t.get_name().to_string();
            // TODO:补全
            com->set_owner_object(this);
            // 若存在对应组件的key，则直接添加com
            if (component_map_.contains(map_key)) {
                component_map_[map_key].push_back(com);
            }
            else {
                component_map_.emplace(map_key, std::vector<Component*>{com});
            }
            return com;
        }

        template<class T>
        T* get_component() {
            T* res = nullptr;
            rttr::type t = rttr::type::get<T>();
            // 必须继承自component
            if (!t.is_derived_from(rttr::type::get<Component>()))
                return nullptr;

            std::string map_key = t.get_name().to_string();
            if (component_map_.contains(map_key)) {
                if (component_map_[map_key].size() > 0)
                    res = component_map_[map_key].front();
            }
            return res;
        }

        template<class T>
        std::vector<T*> get_component_array() {
            const std::vector<T*> res;
            rttr::type t = rttr::type::get<T>();
            // 必须继承自component
            if (!t.is_derived_from(rttr::type::get<Component>()))
                return std::vector<T*>();

            std::string map_key = t.get_name().to_string();
            if (component_map_.contains(map_key)) {
                res = component_map_[map_key];
            }

            return res;
        }


        HierarchyNode* get_parent() const;
        void set_parent(HierarchyNode* parent);

        Level* get_level() const; // 递归地查找它所属的Level

        void remove_all_children();

        void save();

    private:
        void on_add_child(HierarchyNode* child) override;
        void on_remove_child(HierarchyNode* child) override;



        RTTR_REGISTRATION_FRIEND
        RTTR_ENABLE(HierarchyNode, SerializableObject)
    };
} // TinyRenderer

