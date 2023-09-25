//
// Created by gbc on 2023/9/6.
//

#ifndef TERRAIN_RENDERING_SCENE_H
#define TERRAIN_RENDERING_SCENE_H

#include "element/Element.h"
#include "camera/Camera.h"

#include <vector>
#include <memory>

class Scene {
private:
    std::vector<std::shared_ptr<Element>> elements;
    std::shared_ptr<Camera> camera;
public:
    std::shared_ptr<Camera> getCamera() { return camera; }

    void addElement(const std::shared_ptr<Element> &element) { elements.push_back(element); }
};


#endif //TERRAIN_RENDERING_SCENE_H
