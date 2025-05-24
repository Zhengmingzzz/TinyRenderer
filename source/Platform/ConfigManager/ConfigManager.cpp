//
// Created by Administrator on 25-5-17.
//

#include "ConfigManager.h"
#include <fstream>
#include "Resource/GUID/GUID.h"

#define TR_XSTR(s) TR_STR(s)
#define TR_STR(s) #s

ConfigManager& ConfigManager::get_instance()
{
    static ConfigManager configManager;
    return configManager;
}

void ConfigManager::startup()
{
    root_folder_path_ = TR_XSTR(Engine_Root_Path);
    library_folder_path_ = root_folder_path_ / "library";
    objects_folder_path_ = library_folder_path_ / "objects";
    asset_folder_path_ = root_folder_path_ / "asset";
    config_root_folder_path_ = library_folder_path_ / "config";
    level_config_file_path_ = config_root_folder_path_ / "level" / "level config.json";
}

const std::filesystem::path &ConfigManager::get_root_folder_path()
{
    return root_folder_path_;
}

const std::filesystem::path &ConfigManager::get_library_folder_path()
{
    return library_folder_path_;
}

const std::filesystem::path &ConfigManager::get_objects_folder_path()
{
    return objects_folder_path_;
}

const std::filesystem::path &ConfigManager::get_asset_fodder_path()
{
    return asset_folder_path_;
}

const std::filesystem::path &ConfigManager::get_config_root_folder_path()
{
    return config_root_folder_path_;
}

const std::filesystem::path &ConfigManager::get_level_config_file_path() {
    return level_config_file_path_;
}