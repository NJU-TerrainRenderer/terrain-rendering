#pragma once

#include <vector>
#include <memory>
#include "Camera.h"

class Camera;

class CameraListener {
public:
    virtual void onCameraCreate(std::shared_ptr<Camera> camera) = 0;

    virtual void onCameraUpdate(std::shared_ptr<Camera> camera) = 0;
};