//
// Created by Administrator on 25-3-28.
//
#pragma once

#include "IStream.h"
#include "nlohmann/json.hpp"
#include "StreamType.h"

using json = nlohmann::json;
namespace TinyRenderer {
    class JsonStream : public IStream<json> {
    public:
        json Load(const char* path) override;
        bool Save(const char* path, json resource) override;

        TaskResult<json> Asyn_Load(const char* path) override;
        TaskResult<bool> Asyn_Save(const char* path, json resource) override;
    };

    MACRO_STREAM_TYPE(json, JsonStream)
#include "JsonStream.inl"
}
