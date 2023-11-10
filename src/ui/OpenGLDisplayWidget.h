//
// Created by gbc on 2023/10/5.
//

#ifndef TERRAIN_RENDERING_OPENGLDISPLAYWIDGET_H
#define TERRAIN_RENDERING_OPENGLDISPLAYWIDGET_H

#include <QOpenGLWidget>
#include <utility>
#include "scene/camera/CameraListener.h"
#include "scene/Scene.h"

class OpenGLDisplayWidget : public QOpenGLWidget {
private:
    class CameraParser : public CameraListener {
    private:
        QOpenGLWidget *displayWidget;
    public:
        explicit CameraParser(QOpenGLWidget *displayWidget) : displayWidget(displayWidget) {}
//    explicit CameraParser(QOpenGLWidget *displayWidget) {}

        void onCameraUpdate(std::shared_ptr<Camera> camera) override;
    };

    std::shared_ptr<Scene> scene;

    std::shared_ptr<CameraParser> cameraParser;

protected:
    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

public:

    explicit OpenGLDisplayWidget(QWidget *parent = nullptr);

    void setScene(std::shared_ptr<Scene> &newScene);

    void setMVPMatrix();
};


#endif //TERRAIN_RENDERING_OPENGLDISPLAYWIDGET_H
