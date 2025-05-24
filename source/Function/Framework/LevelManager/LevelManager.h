//
// Created by Administrator on 25-5-24.
//
#pragma once
#include <filesystem>
#include <vector>
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
        std::vector<GUID> level_guid_array_; // 所有创建了的level资源
        std::vector<Level*> level_instance_array_; // 所有实例化了的level实例

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


        Level* create_level(const std::string& level_name, const std::filesystem::path& parent_dir);
        void destroy_level(const std::filesystem::path& meta_path);

        Level* load_level(const std::filesystem::path& meta_path);
        Level* load_level(const GUID& guid);
        Level* load_level_async(const std::filesystem::path& meta_path);
        Level* load_level_async(const GUID& guid);

        void unload_level(const std::filesystem::path& meta_path);
        void unload_level(const GUID& guid);
        void unload_level_async(const std::filesystem::path& meta_path);
        void unload_level_async(const GUID& guid);
    private:
        LevelManager() = default;

        void pending_load_level();
    };


} // TinyRenderer
