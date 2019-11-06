#ifndef SERIAL_H
#define SERIAL_H
#include <QObject>
#include <QFile>

class QSerialPort;

class serial
        : public QObject
{
    Q_OBJECT
    // ---

private:
    QSerialPort *sport;
    QFile dbFile;
    QString receivedHeader;
    bool openPort(const QString&);
    bool isHeaderMode;
    int sizeOfData;
    int readDataS;

public:
    serial(QObject *parent = 0);
    void closePort();

private slots:
    void readSerial();
};

#endif // SERIAL_H
