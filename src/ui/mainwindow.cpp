//
// Created by gbc on 2023/7/22.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <fstream>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->fileDialogButton, &QPushButton::clicked, [=]() {
        QString sceneFileString = QFileDialog::getOpenFileName(this, "选取场景", "/");
        if (sceneFileString.isEmpty()) {
            return;
        }

        Json sceneJson;
        try {
            sceneJson = Json::parse(std::ifstream(sceneFileString.toStdString()));
        } catch (nlohmann::json_abi_v3_11_2::detail::parse_error) {
            return;
        }

        auto scene = Scene::deserialize(sceneJson);
        ui->displayWidget->setScene(scene);
        ui->currSceneName->setText(QString::fromStdString(scene->getName()));
    });
    
    ui->displayWidget->update();
}

MainWindow::~MainWindow() {
    delete ui;
}
