#include "serial.h"
#include <QDebug>
#include <QSerialPort>

serial :: serial(QObject *parent)
    : QObject(parent)
{
    // ---
    sport = new QSerialPort();
    cardPort = new QSerialPort();

    sport->setReadBufferSize(0);
    cardPort->setReadBufferSize(0);


    connect(sport, SIGNAL(readyRead()), this, SLOT(readSerial()));
    connect(cardPort, SIGNAL(readyRead()), this, SLOT(readCard()));

    dbFile.setFileName("db.db");

    if(openPort("/dev/serial0", "/dev/ttyUSB0"))
        emit message("Port connected");
    else
        emit message("Port Error");

    isHeaderMode = true;
    sizeOfData = 0;
}


bool serial :: openPort(const QString& port, const QString& cport)
{
    if(sport->isOpen())
        sport->close();
    if(cardPort->isOpen())
        cardPort->close();

    sport->setDataBits(QSerialPort::Data8);
    sport->setParity(QSerialPort::NoParity);
    sport->setBaudRate(115200);
    sport->setPortName(port);

    cardPort->setDataBits(QSerialPort::Data8);
    cardPort->setParity(QSerialPort::NoParity);
    cardPort->setBaudRate(115200);
    cardPort->setPortName(cport);

    if( sport->open(QIODevice::ReadWrite) && cardPort->open(QIODevice::ReadOnly) )
        return true;
    return false;
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
            emit message("Header received");
            //qDebug() << "Received: " << receivedHeader;
            int cnv = receivedHeader.toInt();
            if(cnv > 0)
            {
                isHeaderMode = false;
                sizeOfData = cnv;
                emit message("Getting data ..");
                //qDebug() << "File opened for write ...";
                dbFile.open(QFile::WriteOnly | QFile::Truncate);
                dbFile.close();
                dbFile.open(QFile::WriteOnly | QFile::Append);
                readDataS = 0;
            }
            sport->flush();
            receivedHeader.clear();
        }
    }else
    {
        QByteArray in = sport->readAll();
        readDataS += in.length();
        dbFile.write(in);
        sport->flush();

        if(readDataS >= sizeOfData)
        {
            dbFile.close();
            sizeOfData = 0;
            isHeaderMode = true;
            emit message("Get data done..");
            //qDebug() << "Data write done and saved ";
        }
    }
}

void serial :: readCard()
{
    cardData += cardPort->readAll();
    emit newCard(QString(cardData));
    if(cardData.endsWith("\n"));
    {
        emit newCard(QString(cardData));
        cardData.clear();
    }
}

void serial :: closePort()
{
    sport->close();
    cardPort->close();
}
