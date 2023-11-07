//
// Created by gbc on 2023/10/5.
//

#ifndef TERRAIN_RENDERING_OPENGLDISPLAYWIDGET_H
#define TERRAIN_RENDERING_OPENGLDISPLAYWIDGET_H

#include <QOpenGLWidget>
#include <utility>
#include "scene/camera/CameraListener.h"
#include "scene/Scene.h"

class OpenGLDisplayWidget : public QOpenGLWidget, public CameraListener {
private:
    std::shared_ptr<Scene> scene;
protected:
    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

public:

    explicit OpenGLDisplayWidget(QWidget *parent = nullptr);

    void setScene(std::shared_ptr<Scene> &newScene);

    void onCameraCreate(std::shared_ptr<Camera> camera) override;

    void onCameraUpdate(std::shared_ptr<Camera> camera) override;

    void setMVPMatrix();
};


#endif //TERRAIN_RENDERING_OPENGLDISPLAYWIDGET_H
