//
// Created by gbc on 2023/9/8.
//

#ifndef TERRAIN_RENDERING_TRIANGLE_H
#define TERRAIN_RENDERING_TRIANGLE_H


#include "Element.h"

class Triangle : Element {
private:
    std::vector<Eigen::Vector3f> points;
public:
    Triangle() = default;
    Triangle(const Eigen::Vector3f& point1, const Eigen::Vector3f& point2, const Eigen::Vector3f& point3);

    std::shared_ptr<std::vector<Triangle>> getMesh() override;
};


#endif //TERRAIN_RENDERING_TRIANGLE_H
