#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QBitArray>
#include <QTimer>
#include <MySqlite/mysqlite.h>
#include "Moudle/moudle.h"
#include "GlobalData/globaldata.h"
extern Moudle g_mod[3][129];
class UdpClient : public QObject
{
    Q_OBJECT
public:
    explicit UdpClient(QObject *parent = 0);
    QUdpSocket *m_udpClient;
    //QUdpSocket *m_udpServer;
    QByteArray m_byteArray;     //数据包
    QByteArray m_heartArray;    //心跳包
    QByteArray m_recvArray;    //接收服务器指令包
    QByteArray m_returnArray;    //回应服务器指令包
    uint sendDataCount = 0;
    uint PORT = 6060;
    QString serverIPOrName;
    QHostAddress serverHost;
    QTimer *timer;  //发送数据包定时器
    QTimer *timerHeart; //发送心跳包定时器
    QTimer *timer1;
    MySqlite udp_db;

    void mySleep(int sec);
    uint GetCrc_16(unsigned char *pData, int nLength, unsigned short init, const unsigned short *ptable);
    uint CRC_GetModbus16(unsigned char *pdata, int len);
    uint GetCRCCode(QByteArray data);
    char ConvertHexChar(char ch);
    QByteArray QString2Hex(QString str);
    //void returnServerData(QByteArray ba);
signals:
    void getServerSuccess();
    void sigSoundType(int type);
public slots:
    void init_m_byteArray();    //初始化数据包
    void init_m_heartArray();   //初始化心跳包
    void init_m_returnArray();   //初始化
    void sendOnceData(uint pass,uint startCanId);
    void sendData();    //发送数据包
    void sendHeartData();   //发送心跳包
    void recvServerData();
    void getServerHost();
    void startSendTimer();

};

#endif // UDPCLIENT_H
