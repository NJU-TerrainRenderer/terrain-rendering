#pragma once

#include "Accelerator.h"

class LOD : Accelerator {
public:
    virtual void onCameraCreate(std::shared_ptr<Camera>) override;

    virtual void onCameraUpdate(std::shared_ptr<Camera>) override;

    virtual std::shared_ptr<MeshData> simplify() override;

    virtual void build(std::shared_ptr<Camera>) override;

    virtual bool covered(AABB, std::shared_ptr<Camera>) const override;
};