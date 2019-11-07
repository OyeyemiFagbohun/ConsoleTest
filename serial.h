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
    QString receivedHeader;
    bool isHeaderMode;
    int sizeOfData;
    int readDataS;

    QFile dbFile;

    QSerialPort *cardPort;
    QByteArray cardData;

    bool openPort(const QString&, const QString&);

public:
    serial(QObject *parent = 0);
    void closePort();

private slots:
    void readSerial();
    void readCard();

signals:
    void newCard(QString);
};

#endif // SERIAL_H
