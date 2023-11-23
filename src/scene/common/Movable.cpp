//
// Created by gbc on 2023/9/8.
//

#include "Movable.h"

void Movable::affineTransform(const Eigen::Matrix4f &transformation) {
    direction = transformation * direction;
    position = transformation * position;
}

void Movable::moveTo(Eigen::Vector4f &newPosition) {
    position = newPosition;
}

//void Movable::pointAt(Eigen::Vector4f &newDirection) {
//    direction = newDirection;
//}
