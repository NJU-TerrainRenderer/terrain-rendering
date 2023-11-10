#include <QApplication>
#include <QPushButton>
#include "ui/mainwindow.h"
#include "scene/element/Mesh.h"

void testMesh() {
    MeshData testdata;
    testdata.MeshData(0, 0, 10, 10, "C:/CCode/DEM/ASTGTMV003_N14E034_dem.tif");
}

int main(int argc, char *argv[]) {
    testMesh();
    QApplication a(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    return QApplication::exec();
}
