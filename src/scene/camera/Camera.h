//
// Created by gbc on 2023/9/6.
//

#ifndef TERRAIN_RENDERING_CAMERA_H
#define TERRAIN_RENDERING_CAMERA_H

#include <Eigen/Core>

#include "scene/common/Movable.h"
#include "CameraListener.h"
#include "common/json.h"

class Camera : public Movable {
protected:
    float fov = 0;

    float speed = 1;

    std::vector<std::shared_ptr<CameraListener>> listeners;

    // 获取右向量，该向量应垂直于z轴
    Eigen::Vector4f rightDirection();

public:
    Camera(const Eigen::Vector4f &position, const Eigen::Vector4f &direction, float fov);

    static std::shared_ptr<Camera> deserialize(Json json);

    void setFov(float newFov) { fov = newFov; }

    float getFov() { return fov; }

    void registerListener(const std::shared_ptr<CameraListener> &listener);

    void moveForward(float distance);

    void moveRight(float distance);

    void rotatePrecession(float radius);

    void rotateNutation(float radius);
};


#endif //TERRAIN_RENDERING_CAMERA_H
