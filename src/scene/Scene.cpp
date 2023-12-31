//
// Created by gbc on 2023/9/6.
//

#include <iostream>
#include "Scene.h"

std::shared_ptr<Scene> Scene::deserialize(Json json) {
    auto scene = std::make_shared<Scene>();
    std::cout << json.dump();
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

