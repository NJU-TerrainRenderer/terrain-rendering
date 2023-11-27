#pragma once

#include "Accelerator.h"

class Plain : public Accelerator {
    int init = false;
public:
    virtual void onCameraUpdate(const Mesh* mesh,std::shared_ptr<Camera>) override;

    virtual vector<Triangle> simplify() override;

    virtual void build(std::shared_ptr<Camera>) override;
    virtual void deserializeFrom(Json json) override {
        return;
    }
    virtual bool covered() const override;
};