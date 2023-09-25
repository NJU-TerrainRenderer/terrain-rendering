//
// Created by gbc on 2023/9/6.
//

#ifndef TERRAIN_RENDERING_CAMERA_H
#define TERRAIN_RENDERING_CAMERA_H

#include <Eigen/Core>

#include "../common/Movable.h"
#include "CameraListener.h"

class Camera : public Movable {
private:
    float fov = 0;
    std::vector<std::shared_ptr<CameraListener>> listeners;
public:
    void setFov(float newFov) { fov = newFov; }

    void registerListener(const std::shared_ptr<CameraListener> &listener);
};


#endif //TERRAIN_RENDERING_CAMERA_H
