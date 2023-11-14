#include "Plain.h"


void Plain::onCameraCreate(std::shared_ptr<Camera>camera) {
    //获取相机位置信息
    int minx, maxx, miny, maxy;
    getFieid(camera, minx, maxx, miny, maxy);
    //获取mesh信息
    self->rawMesh->getMesh(minx, maxx, miny, maxy);
    self->simplifiedMesh = self->rawMesh;
}

void Plain::onCameraUpdate(std::shared_ptr<Camera>) {
    //获取相机位置信息
    int minx, maxx, miny, maxy;
    getFieid(camera, minx, maxx, miny, maxy);
    //获取mesh信息
    self->rawMesh->getMesh(minx, maxx, miny, maxy);
    self->simplifiedMesh = self->rawMesh;
}

std::shared_ptr<MeshData> Plain::simplify() {
    return self->simplifiedMesh;
}

void Plain::build(std::shared_ptr<Camera>) {
    return;
}

bool Plain::covered(AABB, std::shared_ptr<Camera>) const {
    return false;
}