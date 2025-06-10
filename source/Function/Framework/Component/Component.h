//
// Created by Administrator on 25-5-24.
//
#pragma once
#include "Function/Framework/Object/Object.h"
#include "Function/Framework/ObjectManager/ObjectManager.h"


namespace TinyRenderer {
    class GameObject;

    class Component: public Object {
        friend class GameObject;
    public:
        void set_owner_object(GameObject *parent);
        GameObject* get_owner_object() const;

    private:
        GameObject* owner_object_ = nullptr;


    public:
        virtual void awake(){};
        virtual void on_enable(){};
        virtual void on_disable(){};
        virtual void start(){};
        virtual void update(){};
        virtual void fixed_update(){};
        virtual void pre_render(){};
        virtual void render(){};
        virtual void post_render(){};
        virtual void on_destroy(){};

        void unload() {
            on_destroy();
            ObjectManager::get_instance().unload_object(this);
        }

        Component() = default;

        Component(GameObject* owner_go) {
            set_owner_object(owner_go);
        }

        ~Component() override;

    private:

        RTTR_ENABLE(Object)
    };
} // TinyRenderer

