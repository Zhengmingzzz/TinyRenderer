//
// Created by Administrator on 25-5-29.
//
#pragma once
#include "Object.h"

namespace TinyRenderer {
    // 表示level go的层级节点
    class HierarchyNode {
    public:
        HierarchyNode() = default;

        virtual void on_add_child(HierarchyNode*) = 0;
        virtual void on_remove_child(HierarchyNode*) = 0;

        RTTR_ENABLE()
    };
} // TinyRenderer

