//
// Created by gbc on 2023/9/8.
//

#include "Triangle.h"

Triangle::Triangle(const Eigen::Vector4f &point1, const Eigen::Vector4f &point2, const Eigen::Vector4f &point3) {
    points.push_back(point1);
    points.push_back(point2);
    points.push_back(point3);
};

std::shared_ptr<std::vector<Triangle>> Triangle::getMesh() {
    auto mesh = std::make_shared<std::vector<Triangle>>();
    mesh->push_back(*this);
    return mesh;
}

void Triangle::affineTransform(Eigen::Matrix4f transformation) {
    Movable::affineTransform(transformation);
    for (int i = 0; i < 3; i++) {
        points[0] = transformation * points[0];
    }
}
