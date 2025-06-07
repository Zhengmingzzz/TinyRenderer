//
// Created by Administrator on 25-5-29.
//
#pragma once
#include "Function/Framework/Object/Object.h"

namespace TinyRenderer {
    // 表示level go的层级节点，用于给GO表示上一级节点地址
    class HierarchyNode {
        // HierarchyNode默认和PrimaryObject在一起，后续如果需要拆分，需要修改to_json的
    public:
        virtual ~HierarchyNode() = default;

        HierarchyNode() = default;

        virtual void on_add_child(HierarchyNode*) = 0;
        virtual void on_remove_child(HierarchyNode*) = 0;

        RTTR_ENABLE()
    };
} // TinyRenderer

