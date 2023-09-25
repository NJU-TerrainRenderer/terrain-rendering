//
// Created by gbc on 2023/9/6.
//

#ifndef TERRAIN_RENDERING_ELEMENT_H
#define TERRAIN_RENDERING_ELEMENT_H

#include <Eigen/Core>
#include <memory>
#include "../common/Movable.h"

class Triangle;

class Element : public Movable {
public:
    virtual std::shared_ptr<std::vector<Triangle>> getMesh() = 0;
};


#endif //TERRAIN_RENDERING_ELEMENT_H
