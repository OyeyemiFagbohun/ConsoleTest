#include "serial.h"
#include <QDebug>
#include <QSerialPort>

serial :: serial(QObject *parent)
    : QObject(parent)
{
    // ---
    sport = new QSerialPort();
    qDebug() << "Console: Serial port openned: " << openPort("/dev/serial0");
    connect(sport, SIGNAL(readyRead()), this, SLOT(readSerial()));
}


bool serial :: openPort(const QString& port)
{
    if(sport->isOpen())
        sport->close();

    sport->setDataBits(QSerialPort::Data8);
    sport->setParity(QSerialPort::NoParity);
    sport->setBaudRate(9600);
    sport->setPortName(port);

    return sport->open(QIODevice::ReadWrite);
}

void serial :: readSerial()
{
    qDebug() << "Read started ...";
    QString receivedHeader;
    QByteArray in = sport->readAll();
    receivedHeader.append(in);
    while (!in.endsWith("\n")) {
        in = sport->readAll();
        receivedHeader.append(in);
    }
    qDebug() << "Read ended ...";
    qDebug() << "Received: " << receivedHeader;
}

void serial :: closePort()
{
    sport->close();
}
