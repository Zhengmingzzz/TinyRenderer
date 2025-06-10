//
// Created by Administrator on 25-6-3.
//
#pragma once
#include "Object.h"

namespace TinyRenderer {
    // 管理序列化与唯一标识
    class PrimaryObject : public Object {
    public:
        PrimaryObject() = default;
        PrimaryObject(const GUID& guid){
            PrimaryObject::set_guid(guid);
        }

        PrimaryObject(const GUID& guid, const std::string& object_name) : PrimaryObject(guid) {
            GUIDReference::get_instance().register_object(guid, this);
            name_ = object_name;
        }

        const GUID& get_guid(){
            return guid_;
        }
        virtual void set_guid(const GUID& guid){
            if (guid_.is_valid())
                GUIDReference::get_instance().unregister_object(guid);
            if (guid.is_valid())
                GUIDReference::get_instance().register_object(guid, this);
            guid_ = guid;
        }

        ~PrimaryObject() override {
            GUIDReference::get_instance().unregister_object(guid_);
        }
    private:
        GUID guid_;

        RTTR_ENABLE(Object)
    };
} // TinyRenderer

