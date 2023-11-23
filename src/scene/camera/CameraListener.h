#pragma once

#include <vector>
#include <memory>

class Camera;

class CameraListener {
public:
    virtual void onCameraUpdate(std::shared_ptr<Camera> camera) = 0;
};