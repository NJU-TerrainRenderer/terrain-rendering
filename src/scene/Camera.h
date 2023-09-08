//
// Created by gbc on 2023/9/6.
//

#ifndef TERRAIN_RENDERING_CAMERA_H
#define TERRAIN_RENDERING_CAMERA_H

#include <cmath>

#include <Eigen/Core>

#include "common/Movable.h"


class Camera : Movable {
private:
    float fov = 0;
public:
    void setFov(float newFov) { fov = newFov; }
};


#endif //TERRAIN_RENDERING_CAMERA_H
