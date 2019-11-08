#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class serial;

class MainWindow :
        public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    serial *s;

public slots:
    void setMessage(QString);
    void newCard(QString);
    void setCurSess(QString);
    void setCurClass(QString);
    void startNewClass(QString instructor, QString course, QString dateTime, int dur);
    void clockStudent(QString name, QString id, QByteArray pasp, QString status);
};

#endif // MAINWINDOW_H
