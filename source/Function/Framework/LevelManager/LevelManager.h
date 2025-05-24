//
// Created by Administrator on 25-5-24.
//
#pragma once
#include <filesystem>
#include "Resource/GUID/GUID.h"

namespace TinyRenderer {

    // 管理level的创建/销毁/加载
    class Level;
    class LevelManager {
    public:

    public:
        Level* active_level_;

    private:
        GUID pending_load_level_guid_;
        std::list<GUID> level_guid_list_; // 所有创建了的level资源
        std::list<Level*> level_instance_list_; // 所有实例化了的level实例

    public:
        LevelManager& operator=(LevelManager&) = delete;
        LevelManager(const LevelManager&) = delete;

        static LevelManager& get_instance() {
            static LevelManager instance;
            return instance;
        }

        void startup();
        void shutdown();

        void tick(float delta_time);
        void save();
        void set_active(const std::filesystem::path& meta_path);
        void set_active(const GUID& guid);
        Level* get_active_level() {
            return active_level_;
        }


        Level* create_level(const std::string& level_name, const std::filesystem::path& parent_dir);
        void destroy_level(const std::filesystem::path& meta_path);

        void load_level(const std::filesystem::path& meta_path);
        void load_level(const GUID& guid);
        void load_level_async(const std::filesystem::path& meta_path);
        void load_level_async(const GUID& guid);

        bool on_unload_level(const GUID& guid);
        void unload_level_async(const std::filesystem::path& meta_path);
        void unload_level_async(const GUID& guid);
    private:
        LevelManager() = default;

        void pending_load_level();
    };


} // TinyRenderer
