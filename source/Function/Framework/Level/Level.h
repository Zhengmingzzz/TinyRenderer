//
// Created by Administrator on 25-5-24.
//
#pragma once
#include "Function/Framework/Object/Object.h"

namespace TinyRenderer {
    class GameObject;

    class Level : public Object {
    public:
        std::vector<GameObject*> gameobject_instance_array_;
    private:

    public:
        Level() = default;
        Level(const GUID& guid) : Object(guid) {}
        Level(const GUID& guid, const std::string& name) : Object(guid, name) {}

        void save() override;
        void tick();

        void unload();

    private:
        RTTR_ENABLE(Object)
    };
} // TinyRenderer

