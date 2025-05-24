//
// Created by Administrator on 25-5-24.
//
#pragma once
#include "Function/Framework/Object/Object.h"


namespace TinyRenderer {
    class GameObject;

    class Component: public Object {
    public:
        TinyRenderer::GameObject* owner_object_;

    private:

    public:
        virtual void awake(){};
        virtual void on_enable(){};
        virtual void on_disable(){};
        virtual void start(){};
        virtual void update(){};
        virtual void pre_render(){};
        virtual void render(){};
        virtual void post_render(){};

    private:

        RTTR_ENABLE(Object)
    };
} // TinyRenderer

