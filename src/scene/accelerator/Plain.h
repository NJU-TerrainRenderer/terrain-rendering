#pragma once

#include "Accelerator.h"

class Plain : public Accelerator {
    vector<Triangle> triangles_raw;

public:
    virtual void onCameraUpdate(const Mesh* mesh,std::shared_ptr<Camera>) override;


    virtual void deserializeFrom(Json json) override {
        return;
    }
};