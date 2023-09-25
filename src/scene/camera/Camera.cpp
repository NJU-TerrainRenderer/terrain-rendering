//
// Created by gbc on 2023/9/6.
//

#include "Camera.h"

void Camera::registerListener(const std::shared_ptr<CameraListener>& listener) {
    listeners.push_back(listener);
}
