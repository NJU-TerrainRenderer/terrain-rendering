//
// Created by gbc on 2023/9/8.
//

#ifndef TERRAIN_RENDERING_TRIANGLE_H
#define TERRAIN_RENDERING_TRIANGLE_H


#include "Element.h"

class Triangle : public Element {
private:
    // 此处值为顶点相对锚点的偏移
    std::vector<Eigen::Vector4f> verticeDisplacements;
public:
    Triangle() = default;

    // 三角形方向为参数三点右手螺旋生成的法向
    Triangle(const Eigen::Vector4f& point1, const Eigen::Vector4f& point2, const Eigen::Vector4f& point3);

    void affineTransform(Eigen::Matrix4f transformation) override;

    std::shared_ptr<std::vector<Triangle>> getMesh() override;
};


#endif //TERRAIN_RENDERING_TRIANGLE_H
