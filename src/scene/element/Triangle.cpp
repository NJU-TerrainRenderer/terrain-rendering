//
// Created by gbc on 2023/9/8.
//

#include "Triangle.h"

Triangle::Triangle(const Eigen::Vector3f& point1, const Eigen::Vector3f& point2, const Eigen::Vector3f& point3) {
    points.push_back(point1);
    points.push_back(point2);
    points.push_back(point3);
};

std::shared_ptr<std::vector<Triangle>> Triangle::getMesh() {
    auto mesh = std::make_shared<std::vector<Triangle>>();
    mesh->push_back(*this);
    return mesh;
}