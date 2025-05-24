//
// Created by Administrator on 25-5-24.
//
#pragma once
#include <filesystem>
#include <rttr/registration>
#include "Function/CommonType/json.h"
#include <unordered_set>

#include "Resource/GUID/GUID.h"

namespace TinyRenderer {
    class Object;
    class GUID;

    // 提供资源的加载 卸载服务
    // 但是无法为已经存在的对象属性实现反序列化
    class AssetManager {
    public:

    private:
        // 序列化时需要的参数，用于记录序列化的根对象和序列化了的对象
        std::unordered_set<GUID> serialized_guids_;
        Object* root_serialized_object_ = nullptr;

    public:
        AssetManager operator=(const AssetManager&) = delete;
        AssetManager(const AssetManager&) = delete;

        static AssetManager& get_instance(){
            static AssetManager assert_mgr;
            return assert_mgr;
        }

        Object* load(const std::filesystem::path& meta_file_path);
        Object* load(const GUID& guid);
        void load_by_path(const std::filesystem::path& file_path, rttr::instance obj);

        bool save(Object* target_object_ptr);
        void save_by_path(const std::filesystem::path& file_path, rttr::instance obj);

        bool save_to_meta(const std::filesystem::path& parent_dir, Object* target_object);

        rttr::variant load_variant(const GUID& guid);

    private:
        AssetManager() = default;
        bool write_json_to_file(const std::filesystem::path& file_path, const ordered_json& target_json);
        json read_json_from_file(const std::filesystem::path& file_path);

    };
} // TinyRenderer