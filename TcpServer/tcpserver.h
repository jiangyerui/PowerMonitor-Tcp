#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDateTime>
class NetMeta{

public:
    qint16 id;
    qint16 net;
    uchar data[18];
};





#include "MySqlite/mysqlite.h"


class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);


    uint passCount;

    uint canIndex;
    uint passIndex;
    QTcpServer *m_tcpServer;
    QTcpSocket *m_tcpSocket;
    QMap<uint,QList<uint> > nodeMap;
    MySqlite db;
private:
    void moduleStatus();
private slots:
    void slotReceiveData();
    void slotNewConnection();
    void slotDisConnect();
//    void slotSendToClient(uint pass,uint id,uint type,uint sts,uint curValue,uint baseValue,uint alarmValue);
    void sendToClient(uint pass, uint id, uint type, uint sts, uint av_1, uint bv_1, uint cv_1, uint av_2, uint bv_2, uint cv_2,
                      uint ai_1 = 0, uint bi_1 = 0, uint ci_1 = 0);
};

#endif // TCPSERVER_H
