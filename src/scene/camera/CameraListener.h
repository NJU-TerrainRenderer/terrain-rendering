#pragma once

#include <vector>
#include <memory>
#include <iostream>

class Camera;

class CameraListener {
protected:
    static std::vector<CameraListener*> listeners;
public:
    CameraListener(bool reg = false) {
        if (reg) {
            listeners.push_back(this);
            std::cout << "register a new CameraListener" << std::endl;
        }
    }
    virtual void onCameraUpdate(std::shared_ptr<Camera> camera) = 0;
};
