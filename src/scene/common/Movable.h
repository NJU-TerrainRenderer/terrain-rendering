//
// Created by gbc on 2023/9/8.
//

#ifndef TERRAIN_RENDERING_MOVABLE_H
#define TERRAIN_RENDERING_MOVABLE_H

#include <Eigen/Dense>

class Movable {
public:
    // 默认仿射变换接口，默认实现仅会作用于锚点和方向
    virtual void affineTransform(Eigen::Matrix4f transformation);
protected:
    Eigen::Vector4f position;
    Eigen::Vector4f direction;
};


#endif //TERRAIN_RENDERING_MOVABLE_H
