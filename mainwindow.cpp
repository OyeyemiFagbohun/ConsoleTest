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
    connect(s, SIGNAL(setCurClass(QString)), this, SLOT(setCurClass(QString)));
    connect(s, SIGNAL(setCurSess(QString)), this, SLOT(setCurSess(QString)));
    connect(s, SIGNAL(startNewClass(QString,QString,QString,int)), this, SLOT(startNewClass(QString,QString,QString,int)));
    connect(s, SIGNAL(clockStudent(QString,QString,QByteArray,QString)), this, SLOT(clockStudent(QString,QString,QByteArray,QString)));
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

void MainWindow :: setCurClass(QString str)
{
    ui->cTitleLab->setText(str);
}

void MainWindow :: setCurSess(QString str)
{
    ui->sessionLabel->setText(str);
}

void MainWindow :: startNewClass(QString instructor, QString course, QString dateTime, int dur)
{
    ui->cLecLab->setText(instructor);
    ui->cTitleLab->setText(course);
    QDateTime tt = QDateTime::fromString(dateTime);
    ui->currDateLab->setText(tt.date().toString());
    ui->currTimeLab->setText(tt.time().toString());
    ui->cDurLab->setText(QString::number(dur));
    ui->statusLabel->setText("Class in progress");
}

void MainWindow :: clockStudent(QString name, QString id, QByteArray pasp, QString status)
{
    ui->nameLabel->setText(name);
    ui->idLabel->setText(id);
    ui->uStatusLab->setText(status);

    QPixmap pix(100, 100);
    pix.loadFromData(pasp, "PNG");
    ui->passport->setPixmap(pix);
}
