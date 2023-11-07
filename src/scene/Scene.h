//
// Created by gbc on 2023/9/6.
//

#ifndef TERRAIN_RENDERING_SCENE_H
#define TERRAIN_RENDERING_SCENE_H

#include "element/Element.h"
#include "camera/Camera.h"

#include "common/json.h"
#include <vector>
#include <memory>

class Scene {
private:
    std::string name;

    std::vector<std::shared_ptr<Element>> elements;
    std::shared_ptr<Camera> camera;
public:
    static std::shared_ptr<Scene> deserialize(Json json);

    std::string getName() { return name; }

    void setCamera(std::shared_ptr<Camera> &newCamera) { camera = newCamera; }

    std::shared_ptr<Camera> getCamera() { return camera; }

    void addElement(const std::shared_ptr<Element> &element) { elements.push_back(element); }

    std::vector<std::shared_ptr<Element>> getElements() { return elements; }
};


#endif //TERRAIN_RENDERING_SCENE_H
