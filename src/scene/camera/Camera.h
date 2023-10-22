//
// Created by gbc on 2023/9/6.
//

#ifndef TERRAIN_RENDERING_CAMERA_H
#define TERRAIN_RENDERING_CAMERA_H

#include <Eigen/Core>

#include "scene/common/Movable.h"
#include "CameraListener.h"

class Camera : public Movable {
protected:
    float width = 64;
    float height = 64;
    float fov = 0;

    float speed = 1;

    std::vector<std::shared_ptr<CameraListener>> listeners;
public:
    Camera(Eigen::Vector4f &position, Eigen::Vector4f &direction, float fov)
            : Movable(position, direction),
              fov(fov) {}

    void setWidth(float newWidth) { width = newWidth; }

    void setHeight(float newHeight) { height = newHeight; }

    void setFov(float newFov) { fov = newFov; }

    void registerListener(const std::shared_ptr<CameraListener> &listener);

    void move(float distance);
};


#endif //TERRAIN_RENDERING_CAMERA_H
