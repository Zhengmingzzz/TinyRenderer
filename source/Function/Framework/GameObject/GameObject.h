//
// Created by Administrator on 25-5-24.
//
#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <rttr/registration>

#include "Function/Framework/Component/Component.h"
#include "Function/Framework/Object/Object.h"

namespace TinyRenderer {
    class Level;

    class GameObject : public Object {
    public:
    private:
        GameObject* parent_ = nullptr;
        std::vector<GameObject*> children_arrray_;

        std::unordered_map<std::string, std::vector<Component*>> component_map_;
        Level* owner_ = nullptr;

    public:
        GameObject() = default;
        GameObject(GUID guid, std::string name) : Object(guid, name) {}

        template<class T>
        T* add_component() {
            rttr::type t = rttr::type::get<T>();
            // 必须继承自component
            if (!t.is_derived_from(rttr::type::get<Component>()))
                return nullptr;

            rttr::variant var = t.create();
            if (!var.is_valid()) {
                var.clear();
            }

            Component* com = var.get_value<Component*>();
            std::string map_key = t.get_name().to_string();
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

        void tick();

        void set_parent(GameObject* parent);

        void add_child(GameObject* child);
        void remove_child(GameObject* child);
        void remove_all_children();

    private:

        RTTR_ENABLE(Object)
    };
} // TinyRenderer

