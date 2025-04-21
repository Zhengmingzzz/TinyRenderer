//
// Created by Administrator on 25-3-29.
//
#pragma once
#include <iostream>

namespace TinyRenderer {
    template<class T>
    struct StreamType {
        static IStream<T>* instance() {
            std::cout << "T is not register in any stream" << std::endl;
            std::cerr << typeid(T).name() << std::endl;
            return nullptr;
        }
    };
}

#define MACRO_STREAM_TYPE(RetType, ClassName) \
    template<> \
    struct StreamType<RetType>{ \
        static IStream<RetType>* instance() { \
        static ClassName instance; \
    return &instance; \
    } \
    };
