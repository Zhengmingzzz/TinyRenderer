//
// Created by Administrator on 25-5-24.
//

#include "GameObject.h"

namespace TinyRenderer {
    void GameObject::tick() {
        for (auto& com_vector : component_map_) {
            for (auto& go_ptr : com_vector.second) {
                //go_ptr->update();
            }
        }
    }

} // TinyRenderer