//
// Created by gbc on 2023/9/8.
//

#include "Triangle.h"

Triangle::Triangle(const Eigen::Vector4f &point1, const Eigen::Vector4f &point2, const Eigen::Vector4f &point3) {
    position = point1;

    verticeDisplacements.emplace_back(point1 - position);
    verticeDisplacements.emplace_back(point2 - position);
    verticeDisplacements.emplace_back(point3 - position);

    direction = Eigen::Vector4f();
    direction << verticeDisplacements[1].head<3>().cross(verticeDisplacements[2].head<3>()).normalized(), 0;
}

std::shared_ptr<std::vector<Triangle>> Triangle::getMesh() {
    auto mesh = std::make_shared<std::vector<Triangle>>();
    mesh->push_back(*this);
    return mesh;
}

void Triangle::affineTransform(Eigen::Matrix4f transformation) {
    Movable::affineTransform(transformation);
    for (int i = 0; i < 3; i++) {
        verticeDisplacements[0] = transformation * verticeDisplacements[0];
    }
}
