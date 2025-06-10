//
// Created by Administrator on 25-5-24.
//
#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "Function/Components/Transform/Transform.h"
#include "Function/Framework/Component/Component.h"
#include "Function/Framework/LevelManager/LevelManager.h"
#include "Function/Framework/HierarchyNode/HierarchyNode.h"
#include "Function/Framework/Object/PrimaryObject.h"
#include "Function/Message/Message.h"

namespace TinyRenderer {
    class Level;

    class GameObject : public PrimaryObject, public HierarchyNode {
        friend Component::~Component();
    public:
        Transform transform_;
        std::list<GameObject*> children_list_;


    private:
        HierarchyNode* parent_node_ = nullptr; // 有可能是GO，也有可能是Level，表示上一层级的节点

        std::unordered_map<std::string, std::vector<Component*>> component_map_;
        bool is_destroyed_ = false;
    public:
        // 默认属于
        static GameObject* create(const std::string& name = "default GO", HierarchyNode* parent_node = nullptr);
        GameObject() {
            transform_.set_owner_object(this);
        }
        GameObject(const GUID& guid, const std::string& name) : PrimaryObject(guid, name) {
            transform_.set_owner_object(this);
        }

        ~GameObject() override {
            is_destroyed_ = true;
            if(parent_node_) {
                parent_node_->on_remove_child(this);
                parent_node_ = nullptr;
            }
        }

        template<class T>
        T* add_component() {
            rttr::type t = rttr::type::get<T>();
            // TODO:还要进行检测，防止重复添加一些不能重复添加的组件
            // 必须继承自component
            if (!t.is_derived_from(rttr::type::get<Component>()))
                return nullptr;

            rttr::variant var = t.create();
            if (!var.is_valid()) {
                var.get_type().destroy(var);
            }

            Component* com = var.get_value<Component*>();
            std::string map_key = t.get_name().to_string();
            if (!com) {
                LOG_WARN("add_component failed when var.get_value<Component*>() GO_GUID:" << this->get_guid().to_string() << " Component:" << t.get_name().to_string());
                return nullptr;
            }
            com->set_owner_object(this);
            // 若存在对应组件的key，则直接添加com
            if (component_map_.contains(map_key)) {
                component_map_[map_key].push_back(com);
            }
            else {
                component_map_.emplace(map_key, std::vector<Component*>{com});
            }
            return var.get_value<T*>();
        }

        template<>
        Transform* add_component() {
            return &transform_;
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
                    res = rttr::rttr_cast<T*>(component_map_[map_key].front());
            }
            return res;
        }

        template<>
        Transform* get_component() {
            return &transform_;
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

        void unload();
        void on_unload_component(Component*); // 移除component子组件

    private:
        void on_add_child(HierarchyNode* child) override;
        void on_remove_child(HierarchyNode* child) override;

        RTTR_REGISTRATION_FRIEND
        RTTR_ENABLE(HierarchyNode, PrimaryObject)
    };
} // TinyRenderer

