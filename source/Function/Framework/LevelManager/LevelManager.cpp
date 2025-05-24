//
// Created by Administrator on 25-5-24.
//

#include "LevelManager.h"

#include "LevelConfig.h"
#include "Platform/ConfigManager/ConfigManager.h"
#include "Function/Framework/Level/Level.h"
#include "Function/Framework/ObjectManager/ObjectManager.h"
#include "Function/Message/Message.h"
#include "Resource/AssetManager/AssetManager.h"
// #include "Resource/AssetManager/AssetManager.h"


namespace TinyRenderer {
    void LevelManager::startup() {
        std::filesystem::path level_cfg_path = ConfigManager::get_instance().get_level_config_file_path();

        // 配置level_config
        LevelConfig level_config;
        AssetManager::get_instance().load_by_path(level_cfg_path, level_config);

        for (auto& level_guid : level_config.level_guid_array_) {
            if (level_guid.is_valid())
                level_guid_array_.push_back(level_guid);
        }
        for (auto& level_instance : level_config.level_instance_array_) {
            if (level_instance) {
                level_instance_array_.push_back(level_instance);
                level_instance->get_active();
            }
        }
        active_level_ = level_config.active_level_;

        // 首次创建场景，不存在active_level
        if (active_level_ == nullptr) {
            active_level_ = create_level("Sample Level", ConfigManager::get_instance().get_asset_fodder_path()/("level"));
        }
    }

    void LevelManager::shutdown() {
        save();
    }

    void LevelManager::set_active(const std::filesystem::path &meta_path) {
        GUID guid = GUID::path_to_guid(meta_path);
        if (guid.is_valid()) {
            set_active(guid);
        }
    }
    void LevelManager::set_active(const GUID &guid) {
        if (!guid.is_valid())
            return;
        Object* obj = GUIDReference::get_instance().get_object(guid);
        // 此时需要激活的level已存在
        if (obj) {
            level_instance_array_.push_back(active_level_);
            active_level_ = rttr::rttr_cast<Level*, Object*>(obj);

            auto res = std::find(level_instance_array_.begin(), level_instance_array_.end(), obj);
            if (res != level_instance_array_.end())
                level_instance_array_.erase(res);
        }
        else {
            LOG_WARN("level not exist instance when set active");
        }
    }


    void LevelManager::save() {
        if (!active_level_)
            return;

        // 手动配置config保存
        LevelConfig level_cfg;
        level_cfg.active_level_ = active_level_;
        for (auto& guid : level_guid_array_) {
            if (guid.is_valid()) {
                level_cfg.level_guid_array_.push_back(guid);
            }
        }
        for (auto& level_instance : level_instance_array_) {
            if (level_instance) {
                level_cfg.level_instance_array_.push_back(level_instance);
            }
        }

        // 保存config文件
        AssetManager::get_instance().save_by_path(ConfigManager::get_instance().get_level_config_file_path(),level_cfg);

        // 逐级调用level保存逻辑
        for (auto& level_ptr : level_instance_array_) {
            if (level_ptr) {
                level_ptr->save();
            }
        }
    }


    Level *LevelManager::create_level(const std::string &level_name, const std::filesystem::path &parent_dir) {
        Level* level_ptr = new Level(GUID::allocate_guid(), level_name);
        if (!level_ptr)
            return nullptr;
        AssetManager::get_instance().save_to_meta(parent_dir, level_ptr);
        AssetManager::get_instance().save(level_ptr);

        level_guid_array_.push_back(level_ptr->get_guid());
        level_instance_array_.push_back(level_ptr);
        return level_ptr;
    }

    void LevelManager::tick(float delta_time) {
        if (pending_load_level_guid_.is_valid())
            pending_load_level();
        set_active(level_instance_array_[0]->get_guid());
        unload_level(level_instance_array_[0]->get_guid());
        load_level(ConfigManager::get_instance().get_asset_fodder_path()/("level")/("2nd Level.Level.json"));
        // create_level("2nd Level", ConfigManager::get_instance().get_asset_fodder_path()/("level"));
    }

    void LevelManager::pending_load_level() {
        // 使用移动语义，直接免了pending_load_level_guid_.clear操作
        GUID pending_load_level_guid(std::move(pending_load_level_guid_));

        if (!pending_load_level_guid.is_valid())
            return;
        // 如果已经存在这个对象，也应该实例化
        if (GUIDReference::get_instance().is_exist(pending_load_level_guid))
            return;

        load_level(pending_load_level_guid);

    }


    Level *LevelManager::load_level(const std::filesystem::path &meta_path) {
        GUID guid = GUID::path_to_guid(meta_path);
        if (guid.is_valid()) {
            return load_level(guid);
        }
        return nullptr;
    }

    Level *LevelManager::load_level(const GUID &guid) {
        // 防止重复实例化
        if (GUIDReference::get_instance().is_exist(guid)) {
            return rttr::rttr_cast<Level*, Object*>(GUIDReference::get_instance().get_object(guid));
        }
        Level* level_ptr = nullptr;
        rttr::variant var = AssetManager::get_instance().load_variant(guid);

        if (!var.is_valid())
            return level_ptr;

        level_ptr = var.get_value<Level*>();
        if (!level_ptr)
            return level_ptr;

        level_instance_array_.push_back(level_ptr);

        return level_ptr;
    }

    void LevelManager::unload_level(const std::filesystem::path &meta_path) {
        GUID guid = GUID::path_to_guid(meta_path);
        if (guid.is_valid()) {
            unload_level(guid);
        }
    }

    void LevelManager::unload_level(const GUID &guid) {
        if (!guid.is_valid())
            return;
        if (guid == active_level_->get_guid()) {
            LOG_WARN("active level can not unload");
            return;
        }
        Object* obj_ptr = GUIDReference::get_instance().get_object(guid);
        bool b = obj_ptr->get_active();
        // 不存在这个实例，直接返回
        Level* level_ptr = rttr::rttr_cast<Level*, Object*>(obj_ptr);
        // 类型不匹配，直接返回
        if (!level_ptr)
            return;

        auto res = std::find(level_instance_array_.begin(), level_instance_array_.end(), level_ptr);
        if (res != level_instance_array_.end())
            level_instance_array_.erase(res);

        // 通知level执行卸载逻辑
        level_ptr->unload();
    }
} // TinyRenderer