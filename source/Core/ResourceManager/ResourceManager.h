//
// Created by Administrator on 25-3-29.
//
#pragma once

#include "Function/CommonType/json.h"
#include "Core/Stream/IStream.h"
#include "Core/ThreadPool/TaskResult.h"
#include <iostream>

namespace TinyRenderer {
    class ResourceManager
    {
    public:
        static ResourceManager& Instance();

        // ͬ����T�����ļ�
        template<typename T>
        T SyncLoad(const char* path);

        // ͬ��дT�����ļ�
        template<typename T>
        bool SyncSave(const char* path, T resource);

        // �첽��T�����ļ�
        template<typename T>
        TaskResult<T> AsynLoad(const char* path);

        // �첽дT�����ļ�
        template<typename T>
        TaskResult<bool> AsynSave(const char* path, T resource);


        static void AsynIO_CallBack() {
            std::cout << "Call AsynIO_CallBack" << std::endl;
        };


    private:
        // ��ȡ��дT�����ļ���Stream����Tδע���򷵻�nullptr
        template<typename T>
        IStream<T>* GetStream();

        static std::mutex mtx;
        ResourceManager(){};

        // map�Ƿ��̰߳�ȫ���ͣ���Ҫ����
        //std::unordered_map<std::string, ResourceType> resource_map_;

    public:
        ResourceManager(ResourceManager const&) = delete;
        ResourceManager& operator=(ResourceManager const&) = delete;
    };
#include "ResourceManager.inl"
}