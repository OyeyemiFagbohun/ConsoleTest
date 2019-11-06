#include "serial.h"
#include <QDebug>
#include <QSerialPort>

serial :: serial(QObject *parent)
    : QObject(parent)
{
    // ---
    sport = new QSerialPort();
    dbFile.setFileName("db.db");
    qDebug() << "Console: Serial port openned: " << openPort("/dev/serial0");
    connect(sport, SIGNAL(readyRead()), this, SLOT(readSerial()));
    isHeaderMode = true;
    sizeOfData = 0;
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
    if(isHeaderMode)
    {
        QByteArray in = sport->readAll();
        receivedHeader.append(in);
        if (in.endsWith("\n\0"))
        {
            receivedHeader.chop(1);
            qDebug() << "Read header ended ...";
            qDebug() << "Received: " << receivedHeader;
            int cnv = receivedHeader.toInt();
            if(cnv > 0)
            {
                isHeaderMode = false;
                sizeOfData = cnv;
                qDebug() << "File opened for write ...";
                dbFile.open(QFile::WriteOnly | QFile::Truncate);
                dbFile.close();
                dbFile.open(QFile::WriteOnly | QFile::Append);
                readDataS = 0;
            }
            receivedHeader.clear();
        }
    }else{
        QByteArray in = sport->readAll();
        readDataS += in.size();
        dbFile.write(in);
        if(readDataS >= sizeOfData)
        {
            dbFile.close();
            sizeOfData = 0;
            isHeaderMode = true;
            qDebug() << "Data write done and saved ";
        }
    }
}

void serial :: closePort()
{
    sport->close();
}
