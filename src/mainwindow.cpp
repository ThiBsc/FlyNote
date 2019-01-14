#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    fnote(new FlyNote("My note", "Hello world!"))
{
    ui->setupUi(this);

    fnote->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
