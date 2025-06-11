//
// Created by Administrator on 25-5-24.
//
#pragma once

#include <rttr/registration>

#include "Core/GUIDReference/GUIDReference.h"
#include "Resource/GUID/GUID.h"
#include <Core/MemoryManager/MemoryManager.h>

#define REGISTRATION_WITH_CONSTRUCTOR_BEGIN(className) \
RTTR_REGISTRATION{ \
using namespace TinyRenderer; \
rttr::registration::class_<className>(#className) \
.constructor([]{return newElement(className);}, rttr::policy::ctor::as_raw_ptr)

#define REGISTRATION_NO_CONSTRUCTOR_BEGIN(className) \
RTTR_REGISTRATION{ \
using namespace TinyRenderer; \
rttr::registration::class_<className>(#className)

#define REGISTRATION_END ;}

// TODO: 需要新增反射属性标识时，在此新增一个宏，并且在to/from json中修改
#define PROPERTY_FLAG_SERIALIZE "SERIALIZE"
#define METADATA_SERIALIZE rttr::detail::metadata(PROPERTY_FLAG_SERIALIZE, true) // 需要实现序列化功能

namespace TinyRenderer {
    // TODO:移除Object的name_字段
    // 万物的基类
    class Object {
    public:
        std::string name_ = "default";


        bool is_active() const {
            return is_active_;
        }

        void set_active(bool active) {
            is_active_ = active;
        }
    private:
        bool is_active_ = true;


    public:
        Object() = default;

        Object(const std::string& name) {
            name_ = name;
        }
        // 获得最终的对象类型
        std::string get_object_type() {
            return rttr::type::get(*this).get_name().to_string();
        }

        virtual ~Object() = 0;

    private:


        RTTR_ENABLE()
    };

} // TinyRenderer

