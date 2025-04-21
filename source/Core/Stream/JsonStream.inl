#pragma once

#include "JsonStream.h"
#include <fstream>
#include <iostream>
#include "Core/ThreadPool/ThreadPool.h"
#include "Core/ResourceManager/ResourceManager.h"

    inline json TinyRenderer::JsonStream::Load(const char* path) {
            std::ifstream ifstr(path);
            json j{};
            if (!ifstr.is_open()){
                std::cerr << "�޷����ļ���" << path << std::endl;
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

    inline bool TinyRenderer::JsonStream::Save(const char* path, json resource) {
        // ʹ��ofstream�����
        std::ofstream ofstr(path);
        if (!ofstr.is_open())
        {
            std::cerr << "�޷����ļ���" << path << std::endl;
            return false;
        }

        // ֱ��д��buffer����
        ofstr << resource.dump(4);
        ofstr.close();

        return true;
    }

    inline TinyRenderer::TaskResult<json> TinyRenderer::JsonStream::Asyn_Load(const char* path) {
        return ThreadPool::instance().enqueue(
        [this, path]()->json {
            json res = Load(path);
            ResourceManager::Instance().AsynIO_CallBack();
            return res;
        },TaskPriority::Medium
    );
    }

    inline TinyRenderer::TaskResult<bool> TinyRenderer::JsonStream::Asyn_Save(const char* path, json resource) {
        return ThreadPool::instance().enqueue(
            [this, path, resource]()->bool {
                bool res = this->Save(path, resource);
                ResourceManager::Instance().AsynIO_CallBack();
                return res;
            }, TaskPriority::MediumHigh);
    }