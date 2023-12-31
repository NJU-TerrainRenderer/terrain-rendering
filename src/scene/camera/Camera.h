//
// Created by gbc on 2023/9/6.
//

#ifndef TERRAIN_RENDERING_CAMERA_H
#define TERRAIN_RENDERING_CAMERA_H

#include <Eigen/Core>

#include "scene/common/Movable.h"
#include "CameraListener.h"
#include "common/json.h"

class Camera : public Movable, public std::enable_shared_from_this<Camera> {
protected:
    float width = 64;
    float height = 64;
    float fov = 0.8;

    float speed = 1;

    Eigen::Vector4f right;

    std::vector<std::shared_ptr<CameraListener>> listeners;

    void notifyListeners();

public:
    Camera(const Eigen::Vector4f& position, const Eigen::Vector4f& direction, float fov);

    static std::shared_ptr<Camera> deserialize(Json json);

    void setWidth(float newWidth) { width = newWidth; }

    void setHeight(float newHeight) { height = newHeight; }

    float getWidth() { return width; }

    float getHeight() { return height; }

    void setFov(float newFov) { fov = newFov; }

    float getFov() { return fov; }

    // 获取相机的方向向量
    Eigen::Vector4f getCameraDirection();

    // 获取 right 向量 (与 z 垂直)
    Eigen::Vector4f rightDirection();

    void registerListener(const std::shared_ptr<CameraListener>& listener);

    void moveForward(float distance);

    void moveRight(float distance);

    void moveHigher(float distance);

    // 进动
    void rotatePrecession(float radius);

    // 章动
    void rotateNutation(float radius);

    Eigen::Matrix4f toCameraMatrix();

    Eigen::Matrix4f toWorldMatrix();
};


#endif //TERRAIN_RENDERING_CAMERA_H