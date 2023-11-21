//
// Created by gbc on 2023/10/5.
//

#include "OpenGLDisplayWidget.h"

#include <cmath>
#include <memory>

// for debug:
#include <iostream>

// 由于版本原因, gluPerspective()这个函数被取消了..., 所以自己实现一个.
void glPerspective(GLdouble fov, GLdouble aspectRatio, GLdouble zNear, GLdouble zFar) {
    //gluPerspective(fov, aspectRatio, zNear, zFar );
    GLdouble rFov = fov; //* M_PI / 180.0; arc, instead of deg.
    glFrustum(-zNear * tan(rFov / 2.0) * aspectRatio,
              zNear * tan(rFov / 2.0) * aspectRatio,
              -zNear * tan(rFov / 2.0),
              zNear * tan(rFov / 2.0),
              zNear, zFar);
}
// glLookAt(), 也需要自行实现.
void glLookAt(float eyeX, float eyeY, float eyeZ,
              float centerX, float centerY, float centerZ,
              float upX, float upY, float upZ) {
    Eigen::Vector3f eye(eyeX, eyeY, eyeZ);
    Eigen::Vector3f center(centerX, centerY, centerZ);
    Eigen::Vector3f up(upX, upY, upZ);
    Eigen::Vector3f f = (center - eye).normalized();
    Eigen::Vector3f r = up.cross(f).normalized();
    Eigen::Vector3f u = f.cross(r).normalized();

    Eigen::Matrix4f viewMatrix;
    viewMatrix << r.x(),  r.y(),  r.z(), -r.dot(eye),
            u.x(),  u.y(),  u.z(), -u.dot(eye),
            -f.x(), -f.y(), -f.z(),  f.dot(eye),
            0.0f,   0.0f,   0.0f,          1.0f;

    glMultMatrixf(viewMatrix.data());
}


void OpenGLDisplayWidget::setScene(std::shared_ptr<Scene> &newScene) {
    scene = newScene;
    scene->getCamera()->registerListener(cameraParser);
}


void OpenGLDisplayWidget::initializeGL() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
}

void OpenGLDisplayWidget::setMVPMatrix() {
    auto camera = scene->getCamera();

    glMatrixMode(GL_PROJECTION); // 投影矩阵
    glLoadIdentity();
    float fov = camera->getFov();
    float w = camera->getWidth();
    float h = camera->getHeight();
    glPerspective(fov, (GLfloat) w / (GLfloat) h, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW); // 模型视图矩阵
    glLoadIdentity();
    Eigen::Vector4f dir = camera->getCameraDirection();
    Eigen::Vector4f eye = camera->getPosition();
    Eigen::Vector3f up = camera->rightDirection().head<3>().cross(dir.head<3>()).normalized();
    glLookAt(eye[0], eye[1], eye[2],
             eye[0] + dir[0], eye[1] + dir[1], eye[2] + dir[2],
             up[0], up[1], up[2]);
}

void OpenGLDisplayWidget::resizeGL(int w, int h) {
    glViewport(0, 0, (GLint) w, (GLint) h); // 设置视口大小
}

void OpenGLDisplayWidget::paintGL() {
    if (scene == nullptr) {
        return;
    }
    setMVPMatrix();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(auto & elem : scene->getElements()) {
        auto mesh = elem->getMesh();
        for(auto& triangle: *mesh){
            glBegin(GL_TRIANGLES);
            auto normal = triangle.getNormal();
            glColor3f(fabs(normal[0]), fabs(normal[1]), fabs(normal[2]));
            for(auto& vertex: *(triangle.getVertices())){
                glVertex3f(vertex[0], vertex[1], vertex[2]);
            }
            glEnd();
        }
    }
}

OpenGLDisplayWidget::OpenGLDisplayWidget(QWidget *parent) :
        QOpenGLWidget(parent) {
    cameraParser = std::make_shared<CameraParser>(this);
}

void OpenGLDisplayWidget::CameraParser::onCameraUpdate(std::shared_ptr<Camera> camera) {
    displayWidget->update();
}