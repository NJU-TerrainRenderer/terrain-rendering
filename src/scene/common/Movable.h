//
// Created by gbc on 2023/9/8.
//

#ifndef TERRAIN_RENDERING_MOVABLE_H
#define TERRAIN_RENDERING_MOVABLE_H

#include <Eigen/Core>

class Movable {
public:
    Eigen::Vector3f position;
    Eigen::Vector3f direction;
};


#endif //TERRAIN_RENDERING_MOVABLE_H
