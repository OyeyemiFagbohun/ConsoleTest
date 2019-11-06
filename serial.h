#ifndef SERIAL_H
#define SERIAL_H
#include <QObject>

class QSerialPort;

class serial
        : public QObject
{
    Q_OBJECT
    // ---

private:
    QSerialPort *sport;
    bool openPort(const QString&);

public:
    serial(QObject *parent = 0);
    void closePort();

private slots:
    void readSerial();
};

#endif // SERIAL_H
