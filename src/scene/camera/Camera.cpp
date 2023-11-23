//
// Created by gbc on 2023/9/6.
//

#include "Camera.h"

Camera::Camera(const Eigen::Vector4f &position, const Eigen::Vector4f &direction, float fov)
        : Movable(position, direction),
          fov(fov) {}

void Camera::registerListener(const std::shared_ptr<CameraListener> &listener) {
    listeners.push_back(listener);
    listener->onCameraUpdate(shared_from_this());
}

void Camera::moveForward(float distance) {
    position += direction * distance;
    notifyListeners();
}

void Camera::moveRight(float distance) {
    position += rightDirection() * distance;
    notifyListeners();
}

void Camera::moveHigher(float distance) {
    position += Eigen::Vector4f(0, 0, 1, 0) * distance;
    notifyListeners();
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

Eigen::Vector4f Camera::getCameraDirection() {
    return getDirection().normalized();
}

void Camera::rotatePrecession(float radius) {
    Eigen::Matrix4f rotation;
    rotation << cosf(radius), -sinf(radius), 0, 0,
            sinf(radius), cosf(radius), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;
    direction = (rotation * direction).normalized();
    notifyListeners();
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
    notifyListeners();
}

void Camera::notifyListeners() {
    for (const std::shared_ptr<CameraListener> &listener: listeners) {
        listener->onCameraUpdate(shared_from_this());
    }
}

Eigen::Matrix4f Camera::toCameraMatrix() {
    Eigen::Vector3f eye = position.head<3>();
    Eigen::Vector3f f = direction.head<3>().normalized();
    Eigen::Vector3f r = rightDirection().head<3>();
    Eigen::Vector3f u = r.cross(f).normalized();

    Eigen::Matrix4f viewMatrix;
    viewMatrix << r.x(), r.y(), r.z(), -r.dot(eye),
            u.x(), u.y(), u.z(), -u.dot(eye),
            -f.x(), -f.y(), -f.z(), f.dot(eye),
            0.0f, 0.0f, 0.0f, 1.0f;

    return viewMatrix;
}

Eigen::Matrix4f Camera::toWorldMatrix() {
    return toCameraMatrix().reverse();
}
