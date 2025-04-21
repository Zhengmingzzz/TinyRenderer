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
                std::cerr << "无法打开文件：" << path << std::endl;
            }
            else {
                try {
                    ifstr >> j;  // 直接通过输入流解析JSON[1,2](@ref)
                    // 或使用 parse 方法：
                    // j = json::parse(ifstr);
                }
                catch (const json::parse_error& e) {
                    std::cerr << "JSON解析错误: " << e.what() << std::endl;
                }
            }
            return j;
    }

    inline bool TinyRenderer::JsonStream::Save(const char* path, json resource) {
        // 使用ofstream输出流
        std::ofstream ofstr(path);
        if (!ofstr.is_open())
        {
            std::cerr << "无法打开文件：" << path << std::endl;
            return false;
        }

        // 直接写入buffer数据
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