//
// Created by Administrator on 25-5-24.
//

#include "AssetManager.h"
#include <fstream>
#include <iostream>

#include "Core/GUIDReference/GUIDReference.h"
#include "Function/Framework/Object/SerializableObject.h"
#include "Resource/GUID/GUID.h"


#include "from_json.h"
#include "to_json.h"
#include "Function/Framework/Object/SerializableObject.h"
#include "Function/Message/Message.h"


namespace TinyRenderer {
    // 通过meta文件里的guid反序列化出object
    Object *AssetManager::load(const std::filesystem::path& meta_file_path) {
        json meta_json = read_json_from_file(meta_file_path);
        std::string guid_type_string = rttr::type::get<GUID>().get_name().to_string();
        GUID guid(meta_json[guid_type_string]);
        // 根据GUID获取路径
        if(!guid.is_valid()){
            std::cerr << meta_file_path << " : " << "guid is not valid" << std::endl;
            return nullptr;
        }

        return load(guid);
    }

    Object* AssetManager::load(const GUID& guid){
        // 防止重复创建用一GUID对应的实例
        if (GUIDReference::get_instance().is_exist(guid))
            return GUIDReference::get_instance().get_object(guid);

        std::filesystem::path guid_file_path = GUID::guid_to_path(guid);

        json res_json = read_json_from_file(guid_file_path);

        rttr::type t = rttr::type::get_by_name(res_json["__type__"]);
        if(!t.is_valid() && !t.is_derived_from<Object>())
            return nullptr;

        // if (t.is_derived_from<Resource>()) {
        //     // TODO:处理 Resource类型
        //
        // }

        // 构建__type__类型实例
        rttr::variant var = t.create();
        io::from_json(res_json, var);

        return var.get_value<Object*>();
    }

    void AssetManager::load_by_path(const std::filesystem::path& file_path, rttr::instance obj) {
        json res_json = read_json_from_file(file_path);

        io::from_json(res_json, obj);
    }

    rttr::variant AssetManager::load_variant(const GUID& guid) {
        // 防止重复创建用一GUID对应的实例
        if (GUIDReference::get_instance().is_exist(guid)) {
            return GUIDReference::get_instance().get_object(guid);
        }

        std::filesystem::path guid_file_path = GUID::guid_to_path(guid);

        json res_json = read_json_from_file(guid_file_path);
        if (res_json.empty())
            return nullptr;

        rttr::type t = rttr::type::get_by_name(res_json["__type__"]);
        if(!t.is_valid() && !t.is_derived_from<Object>())
            return nullptr;

        // if (t.is_derived_from<Resource>()) {
        //     // TODO:处理Resource类型
        // }

        // 对于Object类型处理方法
        rttr::variant var = t.create();
        io::from_json(res_json, var);

        return var;
    }


    // 根据object的guid将它序列化到对应位置
    bool AssetManager::save(SerializableObject* target_object_ptr) {
        if(target_object_ptr == nullptr)
            return false;

        // 获取object的GUID 获取GUID对应的地址 文件保存为GUID.json
        GUID target_guid = target_object_ptr->get_guid();
        if (serialized_objects_.contains(target_guid))
            return true;

        if (root_serialized_object_ == nullptr)
            root_serialized_object_ = target_object_ptr;
        serialized_objects_.insert(target_guid);

        // 开始序列化
        ordered_json resource_json = io::to_json(target_object_ptr);

        std::filesystem::path guid_path = GUID::guid_to_path(target_guid);
        write_json_to_file(guid_path, resource_json);

        // 如果根object处理完退出
        if (target_object_ptr == root_serialized_object_) {
            serialized_objects_.clear();
            root_serialized_object_ = nullptr;
        }
        return true;
    }

    // 将资源保存到指定的位置 用于保存config文件到指定位置
    void AssetManager::save_by_path(const std::filesystem::path& file_path, rttr::instance obj) {
        // 开始序列化
        json resource_json = TinyRenderer::io::to_json(obj);
        // std::cout << resource_json << std::endl;
        write_json_to_file(file_path, resource_json);
    }

    // 将object的GUID和名字等关键信息保存为meta文件
    bool AssetManager::save_to_meta(const std::filesystem::path& parent_dir, SerializableObject* target_object) {
        if(target_object == nullptr)
            return false;

        std::string actual_type = rttr::type::get(*target_object).get_name().to_string();

        // TODO:可以优化metafile序列化逻辑
        ordered_json result_json;
        result_json["type"] = actual_type;
        result_json["name"] = target_object->name_;
        result_json[rttr::type::get<GUID>().get_name().to_string()] = target_object->get_guid().to_string();

        // 最终文件为 parent dir/Object name.Object type.json
        std::filesystem::path meta_file_path = parent_dir / (target_object->name_ + "." + actual_type + ".json");
        return write_json_to_file(meta_file_path, result_json);
    }

    bool AssetManager::write_json_to_file(const std::filesystem::path& file_path, const ordered_json& target_json) {
        if(!exists(file_path)){
            std::filesystem::path dir = file_path.parent_path();
            std::filesystem::create_directories(dir);
        }

        // 创建文件输出流对象
        std::ofstream outfile(file_path);

        // 检查文件是否成功打开
        if (!outfile.is_open()) {
            std::cerr << file_path << " not exist" << std::endl;
            outfile.close();
            return false;
        }

        // 将字符串写入文件
        outfile << target_json.dump(4);

        // 关闭文件流
        outfile.close();

        return true;
    }

    json AssetManager::read_json_from_file(const std::filesystem::path& file_path) {
        // 创建输入文件流对象
        std::ifstream infile(file_path);

        // 检查文件是否成功打开
        if (!infile.is_open()) {
            LOG_WARN(file_path << " not exist");
            infile.close();
            return {};
        }

        json res_json;
        infile >> res_json;

        // 关闭文件流
        infile.close();

        return res_json;
    }
} // TinyRenderer