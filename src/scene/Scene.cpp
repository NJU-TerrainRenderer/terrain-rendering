//
// Created by gbc on 2023/9/6.
//

#include "Scene.h"
#include <iostream>

std::shared_ptr<Scene> Scene::deserialize(Json json) {
    auto scene = std::make_shared<Scene>();
    scene->name = json["name"];
    scene->camera = Camera::deserialize(json["camera"]);
    for (auto &elementJson: json["elements"]) {
        auto element = Element::getElementFromJson(elementJson);
        if (element) {
            scene->addElement(element);
        }
    }
    return scene;
}

