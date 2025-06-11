//
// Created by Administrator on 25-5-24.
//

#include "GameObject.h"
#include "Function/Framework/Level/Level.h"
#include "Function/Framework/ObjectManager/ObjectManager.h"
#include "Function/Message/Message.h"
#include "Resource/AssetManager/AssetManager.h"

REGISTRATION_WITH_CONSTRUCTOR_BEGIN(GameObject)
        .property("parent node",&GameObject::get_parent, &GameObject::set_parent)(
            METADATA_SERIALIZE)
        .property("transform", &GameObject::transform_)(
            METADATA_SERIALIZE)
        .property("children", &GameObject::children_list_)(
            METADATA_SERIALIZE)
        .property("components", &GameObject::component_map_)(
            METADATA_SERIALIZE)
REGISTRATION_END


namespace TinyRenderer {
    HierarchyNode *GameObject::get_parent() const {
        return parent_node_;
    }

    // 通过此接口进行父子层级转换
    void GameObject::set_parent(HierarchyNode* new_parent) {
        // 传入null则把他设置为当前场景根节点
        if (new_parent == nullptr) {
            auto level_ptr = get_level();
            // 默认添加到active_level
            if (level_ptr == nullptr) {
                level_ptr = LevelManager::get_instance().get_active_level();
            }
            level_ptr->on_add_child(this);
            if (parent_node_ != nullptr)
                this->parent_node_->on_remove_child(this);
            parent_node_ = level_ptr;
            return;
        }

        // 如果new_parent为场景
        if (auto level_ptr = dynamic_cast<Level*>(new_parent)) {
            if (auto parent = get_parent()) {
                parent->on_remove_child(this);
            }
            parent_node_ = level_ptr;
            level_ptr->on_add_child(this);
            return;
        }

        // 1.拆分成3个模块 G1_parent->G1 G2
        // G1 G2必须为GameObject
        // TODO:要进行安全判断 有可能未来的HierarchyNode既不是GO也不是Level
        HierarchyNode* G1_parent = this->get_parent();
        GameObject* G1 = this;
        GameObject* G2 = rttr::rttr_cast<GameObject*>(new_parent);

        // 一开始初始化时，G1_parent为空
        if (G1_parent == nullptr) {
            G1->parent_node_ = G2;
            G2->on_add_child(G1);
            return;
        }
        // 将G1和父节点断开联系
        G1->parent_node_ = nullptr;
        G1_parent->on_remove_child(this);

        // 说明传入参数为Level类型，说明需要将G1设置为这个Level的根节点
        if (G2 == nullptr) {
            LOG_ERROR("Failure: set_parent errer : not Hierarchy type " << this->get_guid().to_string());
            return;
        }

        // 反之，参数为GO类型，需要将G2添加到G1_parent下
        // G2和父节点断开连接
        G2->get_parent()->on_remove_child(G2);
        G2->parent_node_ = nullptr;

        // 再重新组装三个模块
        G1_parent->on_add_child(G2);
        G2->parent_node_ = G1_parent;

        G2->on_add_child(G1);
        G1->parent_node_ = G2;
    }

    void GameObject::on_add_child(HierarchyNode *node) {
        if (auto child =  rttr::rttr_cast<GameObject*>(node)) {
            children_list_.push_back(child);
        }

    }

    void GameObject::on_remove_child(HierarchyNode *node) {
        if (auto child =  rttr::rttr_cast<GameObject*>(node)) {
            children_list_.remove(child);
        }
    }

    void GameObject::remove_all_children() {
        children_list_.clear();
    }

    void GameObject::save() {
        AssetManager::get_instance().save(this);
        for (auto go : children_list_) {
            if (go)
                go->save();
        }
    }

    void GameObject::on_unload_component(Component *com) {
        if (com == nullptr)
            return;

        std::string com_name = com->get_object_type();
        if (component_map_.contains(com_name)) {
            auto components_array = component_map_[com_name];
            auto com_it = std::find(components_array.begin(), components_array.end(), com);
            if (com_it != components_array.end()) {
                components_array.erase(com_it);
            }
        }
    }


    GameObject* GameObject::create(const std::string& name, HierarchyNode* parent_node) {
        GameObject* go = new GameObject(GUID::allocate_guid(), name);
        go->set_parent(parent_node);
        return go;
    }

    Level *GameObject::get_level() const {
        const HierarchyNode* current = this;
        while (current) {
            if (auto ptr = rttr::rttr_cast<Level*>(parent_node_)) {
                return ptr;
            }
            current = get_parent();
        }
        return nullptr;
    }

    void GameObject::unload() {
        // set_active(false);
        // 遍历GO的子GO，调用unload
        auto it = children_list_.begin();
        while (it != children_list_.end()) {
            if (*it) {  // 元素非空
                (*it)->unload();
                ++it;
            } else {    // 元素为空
                it = children_list_.erase(it);  // erase返回下一个有效迭代器[2,3,7](@ref)
            }
        }

        // 遍历所有component调用unload
        for (auto com_pair : component_map_) {
            for (auto com : com_pair.second) {
                if (com) {
                    com->unload();
                }
                else {
                    LOG_ERROR("GO:" << this->get_guid().to_string() << " : occur error when foreach component" << com_pair.first << " : but component is null");
                }
            }
        }

        ObjectManager::get_instance().unload_object(this);
    }
} // TinyRenderer