//
// Created by gbc on 2023/9/8.
//

#ifndef TERRAIN_RENDERING_MOVABLE_H
#define TERRAIN_RENDERING_MOVABLE_H

#include <Eigen/Core>

class Movable {
public:
    virtual void affineTransform(Eigen::Matrix4f transformation);
protected:
    Eigen::Vector4f position;
    Eigen::Vector4f direction;
};


#endif //TERRAIN_RENDERING_MOVABLE_H
