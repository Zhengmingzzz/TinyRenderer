//
// Created by Administrator on 25-3-28.
//
#pragma once

#include "IStream.h"
#include "Function/CommonType/json.h"
#include "StreamType.h"
#include <fstream>

#include "Core/ResourceManager/ResourceManager.h"
#include "Core/ThreadPool/ThreadPool.h"
#include "Platform/Path.h"

namespace TinyRenderer {
    class JsonStream : public IStream<json> {
    public:
        json Load(Path&& path) override {
            std::ifstream ifstr(path.toString());
            json j{};
            if (!ifstr.is_open()){
                std::cerr << "�޷����ļ���" << path.toString() << std::endl;
            }
            else {
                try {
                    ifstr >> j;  // ֱ��ͨ������������JSON[1,2](@ref)
                    // ��ʹ�� parse ������
                    // j = json::parse(ifstr);
                }
                catch (const json::parse_error& e) {
                    std::cerr << "JSON��������: " << e.what() << std::endl;
                }
            }
            return j;
        }
        bool Save(Path&& path, json resource) override{
            std::filesystem::create_directory(path.parentPath().toString());

            // ʹ��ofstream�����
            std::ofstream ofstr(path.toString());
            if (!ofstr.is_open())
            {
                std::cerr << "�޷����ļ���" << path.toString() << std::endl;
                return false;
            }

            // ֱ��д��buffer����
            ofstr << resource.dump(4);
            ofstr.close();

            return true;
        }

        bool Save(Path&& path, ordered_json resource) {
            std::filesystem::create_directory(path.parentPath().toString());

            // ʹ��ofstream�����
            std::ofstream ofstr(path.toString());
            if (!ofstr.is_open())
            {
                std::cerr << "�޷����ļ���" << path.toString() << std::endl;
                return false;
            }

            // ֱ��д��buffer����
            ofstr << resource.dump(4);
            ofstr.close();

            return true;
        }

        TaskResult<json> Asyn_Load(Path&& path) override{
            return ThreadPool::instance().enqueue(
            [this, moved_path = std::move(path)]() mutable ->json {
                json res = Load(std::move(moved_path));
                ResourceManager::Instance().AsynIO_CallBack();
                return res;
            },TaskPriority::Medium
        );
        }

        TaskResult<bool> Asyn_Save(Path&& path, json resource) override{
            return ThreadPool::instance().enqueue(
                [this, moved_path = std::move(path), resource]() mutable ->bool {
                    bool res = this->Save(std::move(moved_path), resource);
                    ResourceManager::Instance().AsynIO_CallBack();
                    return res;
                }, TaskPriority::MediumHigh);
        }
    };

    MACRO_STREAM_TYPE(json, JsonStream)
}
