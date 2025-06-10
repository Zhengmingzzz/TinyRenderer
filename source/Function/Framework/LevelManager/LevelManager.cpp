//
// Created by Administrator on 25-5-24.
//

#include "LevelManager.h"

#include "LevelConfig.h"
#include "Function/Components/login/login.h"
#include "Function/Framework/GameObject/GameObject.h"
#include "Platform/ConfigManager/ConfigManager.h"
#include "Function/Framework/Level/Level.h"
#include "Function/Framework/ObjectManager/ObjectManager.h"
#include "Function/Message/Message.h"
#include "Resource/AssetManager/AssetManager.h"


namespace TinyRenderer {
    void LevelManager::startup() {
        std::filesystem::path level_cfg_path = ConfigManager::get_instance().get_level_config_file_path();

        // 配置level_config
        LevelConfig level_config;
        AssetManager::get_instance().load_by_path(level_cfg_path, level_config);

        for (auto& level_guid : level_config.level_guid_array_) {
            if (level_guid.is_valid())
                level_guid_list_.push_back(level_guid);
        }
        for (auto& level_instance : level_config.level_instance_array_) {
            if (level_instance) {
                level_instance_list_.push_back(level_instance);
            }
        }
        active_level_ = level_instance_list_.front();

        // 首次创建场景，不存在active_level
        if (active_level_ == nullptr) {
            active_level_ = Level::create("Sample Level");
        }
    }

    void LevelManager::shutdown() const {
        save();
        auto go = active_level_->root_gameobject_list_.front();
        go->transform_.position_ = glm::vec3(1, 0, 0);
        // 逐级调用level保存逻辑
        auto level_it = level_instance_list_.begin();
        while (level_it != level_instance_list_.end()) {
            if (*level_it) {
                (*level_it)->unload();
                ++level_it;
            }
        }
    }

    void LevelManager::set_active_level(const std::filesystem::path &meta_path) {
        GUID guid = GUID::metafile_path_to_guid(meta_path);
        if (guid.is_valid()) {
            set_active_level(guid);
        }
    }

    void LevelManager::set_active_level(const GUID &guid) {
        if (!guid.is_valid())
            return;
        Object* obj = GUIDReference::get_instance().get_object(guid);
        // 此时需要激活的level已存在
        if (rttr::rttr_cast<Level*, Object*>(obj) != nullptr) {
            active_level_ = rttr::rttr_cast<Level*, Object*>(obj);
            // 将active_level_放到list前面
            level_instance_list_.remove(active_level_);
            level_instance_list_.push_front(active_level_);
        }
        else {
            LOG_WARN("level not exist instance when set active");
        }
    }


    void LevelManager::save() const {
        if (!active_level_)
            return;

        // 手动配置config保存
        LevelConfig level_cfg;
        for (auto& guid : level_guid_list_) {
            if (guid.is_valid()) {
                level_cfg.level_guid_array_.push_back(guid);
            }
        }
        for (auto& level_instance : level_instance_list_) {
            if (level_instance) {
                level_cfg.level_instance_array_.push_back(level_instance);
            }
        }

        // 保存config文件
        AssetManager::get_instance().save_by_path(ConfigManager::get_instance().get_level_config_file_path(),level_cfg);

        // 逐级调用level保存逻辑
        for (auto& level_ptr : level_instance_list_) {
            if (level_ptr) {
                level_ptr->save();
            }
        }
    }

    void LevelManager::on_create_level(Level *level) {
        if (!level)
            return;
        level_guid_list_.push_back(level->get_guid());
        level_instance_list_.push_back(level);
    }


    void LevelManager::tick(float delta_time) {
        if (pending_load_level_guid_.is_valid())
            pending_load_level();

        Level* active_level = active_level_;
        auto parent_go = active_level_->root_gameobject_list_.front();
        auto child_go = parent_go->children_list_.front();

        auto t = parent_go->get_component<login>();

        // auto ptr = parent_go->add_component<login>();
        // ptr->name_ = "second login component";
        // ptr->example_str = "second login example";
    }

    // 延迟到下一帧加载场景
    void LevelManager::pending_load_level() {
        // 使用移动语义，直接免了pending_load_level_guid_.clear操作
        GUID pending_load_level_guid(std::move(pending_load_level_guid_));
        pending_load_level_guid_.clear();

        if (!pending_load_level_guid.is_valid())
            return;
        // 如果已经存在这个对象，也应该实例化
        if (GUIDReference::get_instance().is_exist(pending_load_level_guid))
            return;

        Level* level_ptr = nullptr;
        rttr::variant var = AssetManager::get_instance().load_variant(pending_load_level_guid);

        if (!var.is_valid())
            return;

        level_ptr = var.get_value<Level*>();
        if (!level_ptr)
            return;

        level_instance_list_.push_back(level_ptr);
    }


    void LevelManager::load_level(const std::filesystem::path &meta_path) {
        GUID guid = GUID::metafile_path_to_guid(meta_path);
        if (guid.is_valid()) {
            load_level(guid);
        }
    }

    void LevelManager::load_level(const GUID &guid) {
        // 防止重复实例化
        if (GUIDReference::get_instance().is_exist(guid)) {
            return;
        }
        pending_load_level_guid_ = guid;// 延迟到下一帧加载
    }

    // 由Level调用
    bool LevelManager::on_unload_level(Level* level) {
        if (!level)
            return false;
        level_instance_list_.remove(level);
        return true;
    }

    void LevelManager::unload_level_async(const std::filesystem::path &meta_path) {
        GUID guid = GUID::metafile_path_to_guid(meta_path);
        if (guid.is_valid()) {
            unload_level_async(guid);
        }
    }

    void LevelManager::unload_level_async(const GUID &guid) {
        ObjectManager::get_instance().unload_object(guid);
    }



} // TinyRenderer