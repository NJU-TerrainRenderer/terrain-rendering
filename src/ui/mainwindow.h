//
// Created by gbc on 2023/7/22.
//

#ifndef QT_MINGW_MAINWINDOW_H
#define QT_MINGW_MAINWINDOW_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
};


#endif //QT_MINGW_MAINWINDOW_H
