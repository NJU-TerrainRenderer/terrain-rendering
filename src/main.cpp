#include <QApplication>
#include <QPushButton>
#include "ui/mainwindow.h"
#include "scene/element/Mesh.h"




int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    return QApplication::exec();
}
