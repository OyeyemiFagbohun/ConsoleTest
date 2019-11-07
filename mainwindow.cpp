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
#include <QDesktopWidget>
#include "serial.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    move(QApplication::desktop()->availableGeometry().center() - rect().center());

    s = new serial();
    connect(s, SIGNAL(message(QString)), this, SLOT(setMessage(QString)));
    connect(s, SIGNAL(newCard(QString)), this, SLOT(newCard(QString)));
    //ui->groupBox->setHidden(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow :: setMessage(QString str)
{
    ui->statusLabel->setText(str);
}

void MainWindow :: newCard(QString str)
{
    ui->idLabel->setText(str);
}
