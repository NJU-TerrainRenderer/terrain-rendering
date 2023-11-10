//
// Created by gbc on 2023/9/6.
//

#ifndef TERRAIN_RENDERING_ELEMENT_H
#define TERRAIN_RENDERING_ELEMENT_H

#include <Eigen/Core>
#include <memory>
#include "scene/common/Movable.h"
#include "common/json.h"

class Triangle;

class Element : public Movable {
public:
    static std::shared_ptr<Element> getElementFromJson(Json json);

    virtual void deserializeFrom(Json json) = 0;

    virtual std::shared_ptr<std::vector<Triangle>> getMesh() = 0;

};


#endif //TERRAIN_RENDERING_ELEMENT_H
