//
// Created by gbc on 2023/9/8.
//

#include "Movable.h"

void Movable::affineTransform(Eigen::Matrix4f transformation) {
    direction = transformation * direction;
    position = transformation * position;
}
