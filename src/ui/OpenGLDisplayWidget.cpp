//
// Created by gbc on 2023/10/5.
//

#include "OpenGLDisplayWidget.h"

// 由于版本原因, gluPerspective()这个函数被取消了..., 所以自己实现一个.
void glPerspective(GLdouble fov, GLdouble aspectRatio, GLdouble zNear, GLdouble zFar)
{
    //gluPerspective(fov, aspectRatio, zNear, zFar );
    GLdouble rFov = fov; //* M_PI / 180.0;
    glFrustum( -zNear * tan( rFov / 2.0 ) * aspectRatio,
               zNear * tan( rFov / 2.0 ) * aspectRatio,
               -zNear * tan( rFov / 2.0 ),
               zNear * tan( rFov / 2.0 ),
               zNear, zFar );
}

void OpenGLDisplayWidget::setScene(std::shared_ptr<Scene> &newScene) {
    scene = newScene;
    scene->getCamera()->registerListener(shared_from_this());
}


void OpenGLDisplayWidget::initializeGL() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
}

void OpenGLDisplayWidget::setMVPMatrix() {
    glMatrixMode(GL_PROJECTION); // 投影矩阵
    glLoadIdentity();
    auto camera = scene->getCamera();
    float fov = camera->getFov();
    float w = camera->getWidth();
    float h = camera->getHeight();
    glPerspective(45.0, (GLfloat)w/(GLfloat)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW); // 模型视图矩阵
    glLoadIdentity();
}

void OpenGLDisplayWidget::resizeGL(int w, int h) {
    glViewport(0, 0, (GLint)w, (GLint)h); // 设置视口大小
}

void OpenGLDisplayWidget::paintGL() {
    if(scene == nullptr) {
        return;
    }
    setMVPMatrix();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    // 绘制三角形, (示例)
    glTranslatef(0.0, 0.0, -6.0);
    glRotatef(45, 0.0, 1.0, 0.0);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);
    glEnd();
}

void OpenGLDisplayWidget::onCameraUpdate(std::shared_ptr<Camera> camera) {
    update();
}

OpenGLDisplayWidget::OpenGLDisplayWidget(QWidget *parent) : QOpenGLWidget(parent) {

}
