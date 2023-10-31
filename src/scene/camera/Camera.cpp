//
// Created by gbc on 2023/9/6.
//

#include <iostream>
#include "Camera.h"

Camera::Camera(const Eigen::Vector4f &position, const Eigen::Vector4f &direction, float fov)
        : Movable(position, direction),
          fov(fov) {}

void Camera::registerListener(const std::shared_ptr<CameraListener> &listener) {
    listeners.push_back(listener);
}

void Camera::moveForward(float distance) {
    position += direction * distance;
}

void Camera::moveRight(float distance) {
    position += rightDirection() * distance;
}

std::shared_ptr<Camera> Camera::deserialize(Json json) {
    Eigen::Vector4f position, direction;
    position << json["position"][0], json["position"][1], json["position"][2], 1;
    direction << json["direction"][0], json["direction"][1], json["direction"][2], 0;

    return std::make_shared<Camera>(position, direction.normalized(), json["fov"]);
}

Eigen::Vector4f Camera::rightDirection() {
    Eigen::Vector4f right = {direction[1], -direction[0], 0, 0};
    return right.normalized();
}

void Camera::rotatePrecession(float radius) {
    Eigen::Matrix4f rotation;
    rotation << cosf(radius), -sinf(radius), 0, 0,
            sinf(radius), cosf(radius), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;
    direction = (rotation * direction).normalized();
}

void Camera::rotateNutation(float radius) {
    float r = sqrtf(direction[0] * direction[0] + direction[1] * direction[1]);
    float currNutationRadius = atan2f(direction[2], r);
    float afterNutationRadius = currNutationRadius + radius;
    if (afterNutationRadius >= M_PI_2) {
        direction << 0, 0, 1, 0;
        return;
    }
    if (afterNutationRadius <= -M_PI_2) {
        direction << 0, 0, -1, 0;
        return;
    }

    direction << cosf(afterNutationRadius) * direction[0] / r, cosf(afterNutationRadius) * direction[1] / r,
            sinf(afterNutationRadius), 0;
    direction = direction.normalized();
}
