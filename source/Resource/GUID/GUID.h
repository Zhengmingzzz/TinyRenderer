//
// Created by Administrator on 25-5-15.
//
#pragma once

#include <chrono>
#include <cstdint>
#include <random>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <rttr/registration_friend>
#include <rttr/type.h>
#include <string>
#include "Function/CommonType/json.h"

#include "Platform/ConfigManager/ConfigManager.h"

namespace TinyRenderer {
    class GUID {
        friend class GUIDHash;
    public:
        GUID() = default;
        GUID(const GUID& target_guid){
            this->guid_ = target_guid.guid_;
        }
        GUID(GUID&& target_guid) {
            this->guid_ = std::move(target_guid.guid_);
        }
        GUID(const std::string& guid_string){
            this->guid_ = guid_string;
        }

        bool operator==(const GUID& target) const {
            return guid_ == target.guid_;
        }

        GUID& operator=(const GUID& guid) {  // 返回 GUID& 类型
            if (this == &guid)  // 自赋值检查
                return *this;
            this->guid_ = guid.guid_;
            return *this;  // 返回当前对象的引用
        }

        // 分配新的GUID
        static std::string allocate_guid()
        {
            // 计算当前时间戳
            auto now = std::chrono::system_clock::now();
            uint64_t timestamp_ = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
            uint64_t random_number_ = GenerateRandom();

            std::string string1 = to_hex_string(random_number_);
            std::string string2 = to_hex_string(timestamp_);
            return string1 + '-' + string2;
        }

        std::string to_string() const {
            return guid_;
        }

        void clear()
        {
            guid_.clear();
        }

        bool is_valid() const
        {
            return !guid_.empty();
        }

        static std::filesystem::path guid_to_path(const GUID& guid) {
            if(!guid.is_valid())
                return {};

            // 路径为library/objects/guid前两位名字的文件夹/guid.json
            return ConfigManager::get_instance().get_objects_folder_path() / guid.guid_.substr(0, 2) / (guid.guid_ + ".json");
        }

        static GUID metafile_path_to_guid(const std::filesystem::path& meta_path) {
            std::ifstream infile(meta_path);
            if (!infile.is_open()) {
                std::cerr << "Failed to open file " << meta_path << std::endl;
                return {};
            }
            json meta_file_json;
            infile >> meta_file_json;

            std::string guid_type_string = rttr::type::get<GUID>().get_name().to_string();
            std::string guid_str = meta_file_json[guid_type_string];
            return guid_str;
        }



    private:
        static std::string to_hex_string(uint64_t num)
        {
            char buffer[17];
            snprintf(buffer, sizeof(buffer), "%016llX", num);
            return std::string(buffer);
        }
        static uint64_t GenerateRandom() {
            static thread_local std::mt19937_64 engine(std::random_device{}());
            return engine();
        }
    private:
        std::string guid_ = "";

        RTTR_REGISTRATION_FRIEND
    };

    // struct GUIDHash {
    //     size_t operator()(const GUID& guid) const noexcept {
    //
    //     }
    // };

}
namespace std {
    template<>
    struct hash<TinyRenderer::GUID> {
        size_t operator()(const TinyRenderer::GUID& guid) const noexcept {
            return hash<std::string>()(guid.to_string());
        }
    };
}
