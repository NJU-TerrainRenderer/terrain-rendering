//
// Created by gbc on 2023/9/8.
//

#ifndef TERRAIN_RENDERING_MOVABLE_H
#define TERRAIN_RENDERING_MOVABLE_H

#include <Eigen/Dense>

class Movable {
public:
    // 默认仿射变换接口，默认实现仅会作用于锚点和方向
    virtual void affineTransform(const Eigen::Matrix4f &transformation);

    virtual void moveTo(Eigen::Vector4f &newPosition);

//    virtual void pointAt(Eigen::Vector4f &newDirection);

protected:
    Movable() = default;

    Movable(const Eigen::Vector4f &position, const Eigen::Vector4f &direction)
            : position(position),
              direction(direction) {}

    Eigen::Vector4f position = {0, 0, 0, 1};
    Eigen::Vector4f direction = {1, 0, 0, 0};
};


#endif //TERRAIN_RENDERING_MOVABLE_H
