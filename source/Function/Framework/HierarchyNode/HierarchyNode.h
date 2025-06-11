//
// Created by Administrator on 25-5-29.
//
#pragma once
#include "Function/Framework/Object/PrimaryObject.h"

namespace TinyRenderer {
    // 负责层级关系的管理(GO之间 Level和GO类)
    class HierarchyNode : public PrimaryObject {
        // HierarchyNode默认和PrimaryObject在一起，后续如果需要拆分，需要修改to_json的
    public:
        ~HierarchyNode() override = default;

        HierarchyNode() = default;
        HierarchyNode(const GUID& guid) : PrimaryObject(guid) {
        }
        HierarchyNode(const GUID& guid, const std::string& name) : PrimaryObject(guid, name) {}

        virtual void on_add_child(HierarchyNode*) = 0;
        virtual void on_remove_child(HierarchyNode*) = 0;

        RTTR_ENABLE(PrimaryObject)
    };
} // TinyRenderer

