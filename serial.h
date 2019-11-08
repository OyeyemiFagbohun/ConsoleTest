#ifndef SERIAL_H
#define SERIAL_H
#include <QObject>
#include <QFile>

class QSerialPort;
class QTimer;
#include <QSqlDatabase>
#include <QSqlQueryModel>

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
    QTimer *timer;
    QTimer *dbTimer;

    QFile dbFile;
    QSqlDatabase db;

    QSerialPort *cardPort;
    QByteArray cardData;
    QString currentTable;
    bool dbConnected;
    bool dbInUse;
    bool classStarted;

    bool openPort(const QString&, const QString&);

public:
    serial(QObject *parent = 0);
    void closePort();

private slots:
    void readSerial();
    void readCard();
    void update();
    void updateDb();

signals:
    void newCard(QString);
    void message(QString);
    void setCurSess(QString);
    void setCurClass(QString);
    void startNewClass(QString instructor, QString course, QString dateTime, int dur);
    void clockStudent(QString name, QString id, QByteArray pasp, QString status);
};

#endif // SERIAL_H
