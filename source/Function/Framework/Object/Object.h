//
// Created by Administrator on 25-5-24.
//
#pragma once

#include "Core/GUIDReference/GUIDReference.h"
#include "Resource/GUID/GUID.h"
#include <rttr/registration>

#define REGISTRATION_WITH_CONSTRUCTOR_BEGIN(className) \
RTTR_REGISTRATION{ \
using namespace TinyRenderer; \
rttr::registration::class_<className>(#className) \
.constructor([]{return new className();}, rttr::policy::ctor::as_raw_ptr)

#define REGISTRATION_NO_CONSTRUCTOR_BEGIN(className) \
RTTR_REGISTRATION{ \
using namespace TinyRenderer; \
rttr::registration::class_<className>(#className)

#define REGISTRATION_END ;}

// TODO: 需要新增反射属性标识时，在此新增一个宏，并且在to/from json中修改
#define PROPERTY_FLAG_SERIALIZE "SERIALIZE"
#define METADATA_SERIALIZE rttr::detail::metadata(PROPERTY_FLAG_SERIALIZE, true) // 需要实现序列化功能

#define PROPERTY_FLAG_GUIDTOOBJECT "GUIDTOOBJECT"
#define METADATA_GUIDTOOBJECT rttr::detail::metadata(PROPERTY_FLAG_GUIDTOOBJECT, true) // 反序列化时需要将GUID转为object*


namespace TinyRenderer {
    class Object {
    public:
        const GUID& get_guid(){
            return guid_;
        }
        void set_guid(const GUID& guid){
            if (guid_.is_valid())
                GUIDReference::get_instance().unregister_object(guid);
            if (guid.is_valid())
                GUIDReference::get_instance().register_object(guid, this);
            guid_ = guid;
        }

        std::string name_ = "default";

        bool get_active() {
            return is_active_;
        }
        void set_active(bool active) {
            is_active_ = active;
        }
    private:
        GUID guid_;
        bool is_active_ = true;


    public:
        Object() = default;
        Object(const GUID& guid){
            if(!guid.is_valid())
                return;
            guid_ = guid;

            GUIDReference::get_instance().register_object(guid_, this);
        }

        Object(const GUID& guid, const std::string& name) {
            if(guid.is_valid()) {
                guid_ = guid;
            }
            name_ = name;

        }

        virtual ~Object(){
            GUIDReference::get_instance().unregister_object(guid_);
        }

    private:


        RTTR_ENABLE()
    };

} // TinyRenderer

