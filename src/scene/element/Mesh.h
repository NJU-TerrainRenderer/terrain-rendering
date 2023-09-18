#pragma once
#include"Element.h"
#include"Triangle.h"

class MeshData : Element{
    std::vector<Triangle>triangles;
public:
    MeshData();
    MeshData(std::vector<Triangle>triangles) {};
    bool insertTriangle(Triangle t);
    bool insertTriangle(const Eigen::Vector3f& point1, const Eigen::Vector3f& point2, const Eigen::Vector3f& point3);
    std::shared_ptr<std::vector<Triangle>> getMesh() override;
};