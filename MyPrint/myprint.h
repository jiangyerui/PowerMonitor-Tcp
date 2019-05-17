#ifndef MYPRINT_H
#define MYPRINT_H

#include <QDebug>
#include <QObject>
#include <QByteArray>
#include <QMessageBox>
#include <QTextCodec>
//#include <QSerialPort>//jiang
#include "MySqlite/mysqlite.h"


#define LF      0x0A //进纸命令
//初始化命令
#define ESC_AT   0x1B
#define ESC_AT2  0x40
//汉字模式命令
#define FS_ADD   0x1C
#define FS_ADD2  0x26

class MyPrint : public QObject
{
    Q_OBJECT
public:

    char lf[2];
    MySqlite m_db;
    static MyPrint *print;
//    QSerialPort *myCom;//jiang

    void initSerialPort(const QString &name);
    static MyPrint *getPrint();
    static void deletePrint();
    void checkSelfPrint(QString nodeNum,QString alarmNum, QString errorNum, QString power, QString bpower);
    void printConnect(QString net, QString id, QString status, QString time, QString address);
    void printConnect(QString net, QString id, QString type, QString status, QString value, QString time, QString address);

signals:
    void showNoPageMessage();

public slots:
};

#endif // MYPRINT_H
