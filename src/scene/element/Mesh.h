#pragma once

#include"Element.h"
#include"Triangle.h"
#include<vector>

class MeshData : public Element {
    //std::vector<Triangle>triangles;
public:
    MeshData();

    //MeshData(std::vector<Triangle>triangles);
    //bool insertTriangle(Triangle t);
    //bool insertTriangle(const Eigen::Vector3f& point1, const Eigen::Vector3f& point2, const Eigen::Vector3f& point3);
    virtual std::shared_ptr<std::vector<Triangle>> getMesh() override {
        Triangle t;
        return t.getMesh();
    };
};