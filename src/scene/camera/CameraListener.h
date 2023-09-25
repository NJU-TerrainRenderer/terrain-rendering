#pragma once

#include <vector>
#include <memory>
#include "Camera.h"

class CameraListener {
public:
    virtual void onCameraCreate(std::shared_ptr<Camera>) = 0;

    virtual void onCameraUpdate(std::shared_ptr<Camera>) = 0;
};