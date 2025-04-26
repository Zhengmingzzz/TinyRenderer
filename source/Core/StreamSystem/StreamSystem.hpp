//
// Created by Administrator on 25-4-26.
//
#pragma once
#include <cxxabi.h>
#include <fstream>

#include "Function/Message/Message.h"
#include "Platform/Path.h"
#include "Core/ThreadPool/ThreadPool.h"

namespace TinyRenderer{
    template<class T>
    class IStream {
    public:
        // 文件同步读写API
        virtual T load(Path&& path) = 0;
        virtual bool save(Path&& path, T resource) = 0;

        // 文件异步读写API
        virtual TaskResult<T> async_load(Path&& path) = 0;
        virtual TaskResult<bool> async_save(Path&& path, T resource) = 0;

        virtual ~IStream() = default;
    };
}
// 通过模版特化定义返回值为RetType的类名为ClassName类型的Stream类型对象实例
#define MACRO_STREAM_TYPE(RetType, ClassName) \
    template<> \
    struct StreamType<RetType>{ \
    static IStream<RetType>* instance() { \
        static ClassName instance; \
        return &instance; \
    } \
};

namespace TinyRenderer {
    template<class T>
        struct StreamType {
        static IStream<T>* instance() {
            int status;
            char* demangled = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
            LOG_ERROR("Type " << (demangled ? demangled : "unknown") << " is not registered in any stream");
            free(demangled);
            return nullptr;
        }
    };
}

namespace TinyRenderer {
    class JsonStream : public IStream<json> {
    public:
        json load(Path&& path) override {
            std::ifstream ifstr(path.toString());
            json j{};
            if (!ifstr.is_open()){
                std::cerr << "无法打开文件：" << path.toString() << std::endl;
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
        bool save(Path&& path, json resource) override{
            std::filesystem::create_directory(path.parentPath().toString());

            // 使用ofstream输出流
            std::ofstream ofstr(path.toString());
            if (!ofstr.is_open())
            {
                std::cerr << "无法打开文件：" << path.toString() << std::endl;
                return false;
            }

            // 直接写入buffer数据
            ofstr << resource.dump(4);
            ofstr.close();

            return true;
        }
        bool save(Path&& path, ordered_json resource) {
            std::filesystem::create_directory(path.parentPath().toString());

            // 使用ofstream输出流
            std::ofstream ofstr(path.toString());
            if (!ofstr.is_open())
            {
                std::cerr << "无法打开文件：" << path.toString() << std::endl;
                return false;
            }

            // 直接写入buffer数据
            ofstr << resource.dump(4);
            ofstr.close();

            return true;
        }

        TaskResult<json> async_load(Path&& path) override{
            return ThreadPool::instance().enqueue(
            [this, moved_path = std::move(path)]() mutable ->json {
                json res = load(std::move(moved_path));
                return res;
            },TaskPriority::Medium
        );
        }

        TaskResult<bool> async_save(Path&& path, json resource) override{
            return ThreadPool::instance().enqueue(
                [this, moved_path = std::move(path), resource]() mutable ->bool {
                    bool res = this->save(std::move(moved_path), resource);
                    return res;
                }, TaskPriority::MediumHigh);
        }
    };

    MACRO_STREAM_TYPE(json, JsonStream);

}
