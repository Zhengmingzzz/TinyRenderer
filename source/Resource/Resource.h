//
// Created by Administrator on 25-6-5.
//
#pragma once
#include "Function/Framework/Object/PrimaryObject.h"

namespace TinyRenderer {
    // 所有资源的基类
    class Resource : public PrimaryObject {
    public:
    private:
    public:
        // 资源类型都必须重写set_guid
        void set_guid(const GUID &guid) override {
            PrimaryObject::set_guid(guid);
        }

        ~Resource() override = 0; // 将Resource设置为抽象类
    private:

        RTTR_ENABLE(PrimaryObject)
    };
} // TinyRenderer
