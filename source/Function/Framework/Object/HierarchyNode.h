//
// Created by Administrator on 25-5-29.
//
#pragma once
#include "Object.h"

namespace TinyRenderer {
    // 表示level go的层级节点
    class HierarchyNode : public Object {
    public:
        HierarchyNode() = default;
        HierarchyNode(const GUID& guid) : Object(guid) {}
        HierarchyNode(const GUID& guid, const std::string& name) : Object(guid, name) {}

        virtual void on_add_child(HierarchyNode*) = 0;
        virtual void on_remove_child(HierarchyNode*) = 0;


        RTTR_ENABLE(Object)
    };
} // TinyRenderer

