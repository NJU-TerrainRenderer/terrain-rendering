//
// Created by gbc on 2023/10/5.
//

#include "OpenGLDisplayWidget.h"

void OpenGLDisplayWidget::initializeGL() {
    QOpenGLWidget::initializeGL();
}

void OpenGLDisplayWidget::resizeGL(int w, int h) {

}

void OpenGLDisplayWidget::paintGL() {

}

void OpenGLDisplayWidget::onCameraCreate(std::shared_ptr<Camera> camera) {
    paintGL();
}

void OpenGLDisplayWidget::onCameraUpdate(std::shared_ptr<Camera> camera) {
    paintGL();
}

OpenGLDisplayWidget::OpenGLDisplayWidget(QWidget *parent) : QOpenGLWidget(parent) {

}