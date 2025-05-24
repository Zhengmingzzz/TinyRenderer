//
// Created by Administrator on 25-5-24.
//
#pragma once
#include <vector>
#include "Resource/GUID/GUID.h"

namespace TinyRenderer {
class Object;
    // 负责object的缓存 销毁工作

class ObjectManager {
public:

private:
    std::vector<Object*> unload_object_array_;
public:
    ObjectManager& operator=(ObjectManager&) = delete;
    ObjectManager(ObjectManager&) = delete;

    static ObjectManager& get_instance() {
        static ObjectManager instance;
        return instance;
    }

    void unload_object(Object* object);
    void unload_object(const GUID& object_guid);

    void tick();
private:
    ObjectManager() = default;

};

} // TinyRenderer
