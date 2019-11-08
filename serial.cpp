#include "serial.h"
#include <QDebug>
#include <QSerialPort>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QTimer>
#include <QDateTime>

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
    dbConnected = false;
    dbInUse = false;
    classStarted = false;

    const QString DRIVER("QSQLITE");
    if(QSqlDatabase::isDriverAvailable(DRIVER))
    {
        db = QSqlDatabase::addDatabase(DRIVER);
        db.setDatabaseName("db.db");
    }

    if(openPort("/dev/serial0", "/dev/ttyUSB0"))
        emit message("Port connected");
    else
        emit message("Port Error");

    isHeaderMode = true;
    sizeOfData = 0;

    dbTimer = new QTimer();
    connect(dbTimer, SIGNAL(timeout()), this, SLOT(updateDb()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    dbTimer->start(1700);
    timer->start(3000);
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
    cardPort->setBaudRate(9600);
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
                db.close();
                dbInUse = true;
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
            dbInUse = false;
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
        cardData.chop(1);
        //emit newCard(QString(cardData));
        QString card = QString(cardData);
        cardData.clear();
        if(classStarted && !dbInUse && db.isOpen())
        {
            QSqlQuery query;
            query.exec(QString("SELECT sName, sID, sPassport FROM '%1' WHERE sCardID='%2' ").arg(currentTable).arg(card));
            if(query.next())
            {
                if(query.record().count() == 3)
                {
                    QString name = query.record().value(0).toString();
                    QString id = query.record().value(1).toString();
                    QByteArray pasp = query.record().value(2).toByteArray();
                    emit clockStudent(name, id, pasp, "Success");
                }else{
                    emit clockStudent("", card, QByteArray(), "Error");
                }
            }else{
                emit clockStudent("", card, QByteArray(), "Not registered");
            }
        }else{
            emit clockStudent("", card, QByteArray(), "No Class");
        }
    }
}

void serial :: closePort()
{
    sport->close();
    cardPort->close();
}

void serial :: update()
{
    cardPort->close();
    cardPort->setPortName("/dev/ttyUSB0");

    if(!cardPort->open(QIODevice::ReadWrite))
    {
        cardPort->setPortName("/dev/ttyUSB1");
        cardPort->open(QIODevice::ReadWrite);
    }
}

void serial :: updateDb()
{
    if(dbInUse)
        return;

    if(!db.isOpen())
    {
        db.open();
        if(!db.isOpen());
        else{
            QSqlQuery query; //Start new SQL query to be executed
            query.exec("SELECT name FROM sqlite_master WHERE type='table';");
            if( query.next() )
            {
                QSqlRecord record = query.record();
                currentTable = record.value(0).toString();
                QStringList l = record.value(0).toString().split("%");
                if(l.length() == 2)
                {
                    emit setCurClass(l.at(0));
                    emit setCurSess(l.at(1));
                    dbConnected = true;
                }else
                    db.close();
            }else
                db.close();
        }
    }else{
        QSqlQuery query;
        query.exec(QString("SELECT sDT, cDuration, cInstructor FROM '%1'").arg(currentTable));
        if(query.next())
        {
            QString cTime = query.record().value(0).toString();
            int duration = query.record().value(1).toInt();
            QString cInstr = query.record().value(2).toString();
            if(QDateTime::fromString(cTime) >= QDateTime::currentDateTime() && QDateTime::fromString(cTime).addSecs(60 * 60 * duration) < QDateTime::currentDateTime())
            {
                if(!classStarted)
                {
                    QString cCourse = currentTable.split("%").at(0);
                    emit startNewClass(cInstr, cCourse, cTime, duration);
                    classStarted = true;
                }

            }else{
                if(classStarted)
                {
                    emit startNewClass("", "", "", 0);
                }
                classStarted = false;
            }
        }
    }
}


