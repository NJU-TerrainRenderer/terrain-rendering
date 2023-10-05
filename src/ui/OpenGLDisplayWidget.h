//
// Created by gbc on 2023/10/5.
//

#ifndef TERRAIN_RENDERING_OPENGLDISPLAYWIDGET_H
#define TERRAIN_RENDERING_OPENGLDISPLAYWIDGET_H

#include <QOpenGLWidget>
#include "../scene/camera/CameraListener.h"

class OpenGLDisplayWidget : public QOpenGLWidget, public CameraListener {
protected:
    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

public:
    explicit OpenGLDisplayWidget(QWidget *parent = nullptr);

    void onCameraCreate(std::shared_ptr<Camera> camera) override;

    void onCameraUpdate(std::shared_ptr<Camera> camera) override;
};


#endif //TERRAIN_RENDERING_OPENGLDISPLAYWIDGET_H
