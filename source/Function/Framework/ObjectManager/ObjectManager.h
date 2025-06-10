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
    std::list<Object*> pending_unload_object_list_;
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
    void shutdown();
private:
    ObjectManager() = default;

};

} // TinyRenderer
