//
// Created by Administrator on 25-5-24.
//
#pragma once

#include "Resource/GUID/GUID.h"

namespace TinyRenderer {
    class Object;

    class GUIDReference {
public:
    static GUIDReference& get_instance(){
        static GUIDReference guid_reference;
        return guid_reference;
    }

    bool is_exist(const GUID& target_guid){
        return object_instance_map_.contains(target_guid);
    }

    void register_object(const GUID& object_guid, Object* object_ptr);

    void unregister_object(const GUID& object_guid){
        object_instance_map_.erase(object_guid);
    }

    Object* get_object(const GUID& object_guid) {
        if (object_instance_map_.contains(object_guid))
            return object_instance_map_[object_guid];
        return nullptr;
    }

private:
    std::unordered_map<GUID, Object*> object_instance_map_; // 所有GUID到object实例的映射

public:
    GUIDReference operator=(GUIDReference&) = delete;
    GUIDReference(const GUIDReference&) = delete;

private:
    GUIDReference() = default;
};

} // TinyRenderer
