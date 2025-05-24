//
// Created by Administrator on 25-5-17.
//
#pragma once
#include <filesystem>

class ConfigManager {
public:
    static ConfigManager& get_instance();
private:
    std::filesystem::path root_folder_path_;
    std::filesystem::path library_folder_path_;
    std::filesystem::path objects_folder_path_;
    std::filesystem::path asset_folder_path_;
    std::filesystem::path config_root_folder_path_;
    std::filesystem::path level_config_file_path_;


public:
    void startup();
    const std::filesystem::path& get_root_folder_path();
    const std::filesystem::path& get_library_folder_path();
    const std::filesystem::path& get_objects_folder_path();
    const std::filesystem::path& get_asset_fodder_path();
    const std::filesystem::path& get_config_root_folder_path();
    const std::filesystem::path& get_level_config_file_path();


private:
};

