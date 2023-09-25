#include "Plain.h"

void Plain::onCameraCreate(std::shared_ptr<Camera>) {
    //TODO
}

void Plain::onCameraUpdate(std::shared_ptr<Camera>) {
    //TODO
}

std::shared_ptr<MeshData> Plain::simplify() {
    return NULL;
}

void Plain::build(std::shared_ptr<Camera>) {
    return;
}

bool Plain::covered(AABB, std::shared_ptr<Camera>) const {
    return false;
}