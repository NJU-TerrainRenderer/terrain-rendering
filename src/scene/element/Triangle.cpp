//
// Created by gbc on 2023/9/8.
//

#include "Triangle.h"

Triangle::Triangle(Eigen::Vector4f &point1, Eigen::Vector4f &point2, Eigen::Vector4f &point3) {
    position = point1;

    vertexDisplacements.emplace_back(point1 - position);
    vertexDisplacements.emplace_back(point2 - position);
    vertexDisplacements.emplace_back(point3 - position);

    direction << vertexDisplacements[1].head<3>().cross(vertexDisplacements[2].head<3>()).normalized(), 0;
}

std::shared_ptr<std::vector<Triangle>> Triangle::getMesh() {
    auto mesh = std::make_shared<std::vector<Triangle>>();
    mesh->push_back(*this);
    return mesh;
}

void Triangle::affineTransform(const Eigen::Matrix4f &transformation) {
    Movable::affineTransform(transformation);
    for (int i = 0; i < 3; i++) {
        vertexDisplacements[0] = transformation * vertexDisplacements[0];
    }
}

void Triangle::deserializeFrom(Json json) {
    position << json["points"][0][0], json["points"][0][1], json["points"][0][2], 1;

    Eigen::Vector4f point;
    for (auto &pointJson: json["points"]) {
        point << pointJson[0], pointJson[1], pointJson[2], 1;
        vertexDisplacements.emplace_back(point - position);
    }
    direction << vertexDisplacements[1].head<3>().cross(vertexDisplacements[2].head<3>()).normalized(), 0;
}

std::shared_ptr<std::vector<Eigen::Vector4f>> Triangle::getVertices() {
    auto vertices = std::make_shared<std::vector<Eigen::Vector4f>>();
    for (const auto& vertexDisplacement: vertexDisplacements) {
        vertices->emplace_back(position + vertexDisplacement);
    }
    return vertices;
}
