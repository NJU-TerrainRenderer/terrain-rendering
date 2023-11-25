#pragma once

#include "Accelerator.h"

class Plain : Accelerator {
    int init = false;
public:
    virtual void onCameraUpdate(const MeshData& mesh,std::shared_ptr<Camera>) override;

    virtual vector<Triangle> simplify() override;

    virtual void build(std::shared_ptr<Camera>) override;

    virtual bool covered() const override;
};