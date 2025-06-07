//
// Created by Administrator on 25-5-24.
//
#pragma once
#include <filesystem>
#include <rttr/registration>
#include "Function/CommonType/json.h"

#include "Resource/GUID/GUID.h"

namespace TinyRenderer {
    class PrimaryObject;
    class Object;
    class GUID;

    // 提供资源的加载 卸载服务
    // TODO:AssetManager职责过重，需要拆分为多个模块
    // TODO:GUIDTOOBJECT似乎没什么用，尝试删除
    // TODO:尝试只注册Component*，看看反序列化中调用create时候，能否成功生成vector<Component*>
    class AssetManager {
    public:

    private:
        // 序列化时需要的参数，用于记录序列化的根对象和序列化了的对象
        // std::unordered_set<GUID> serialized_objects_;
        // Object* root_serialized_object_ = nullptr;

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

        bool save(PrimaryObject* target_object_ptr);
        void save_by_path(const std::filesystem::path& file_path, rttr::instance obj);

        bool save_to_meta(const std::filesystem::path& parent_dir, PrimaryObject* target_object);

        rttr::variant load_variant(const GUID& guid);

    private:
        AssetManager() = default;
        bool write_json_to_file(const std::filesystem::path& file_path, const ordered_json& target_json);
        json read_json_from_file(const std::filesystem::path& file_path);

    };
} // TinyRenderer