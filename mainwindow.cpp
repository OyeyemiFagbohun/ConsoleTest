#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QStandardPaths>
#include <QFileDialog>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    move(QApplication::desktop()->availableGeometry().center() - rect().center());

    ui->groupBox->setHidden(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
