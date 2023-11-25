//
// Created by gbc on 2023/9/6.
//

#include "Element.h"
#include "Mesh.h"
#include "Triangle.h"

std::shared_ptr<Element> Element::getElementFromJson(Json json) {
    std::string type = json["type"];

    std::shared_ptr<Element> element;
    if (type == "triangle") {
        element = std::make_shared<Triangle>();
    }
    else if (type == "mesh") {
        element = std::make_shared<Mesh>();
    }
    else {
        return element;
    }
    element->deserializeFrom(json);
    return element;
}
