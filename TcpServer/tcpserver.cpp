#include "tcpserver.h"
#include "GlobalData/globaldata.h"
#include <QThread>

#define CMDALL 0x01


#define SIZE    16


#define HEAD_AA 0
#define HEAD_00 1
#define PASS    2
#define CANID   3
#define TYPE    4
#define STS     5
//电压1
#define _AV_1    6
#define _BV_1    7
#define _CV_1    8
//电压2
#define _AV_2    9
#define _BV_2    10
#define _CV_2    11
//电流1
#define _AI_1    12
#define _BI_1    13
#define _CI_1    14
#define CRC     15





#define ALLSTS 0x01
#define ONESTS 0x04

#define SER         0x0B
#define DATACMD     0x01

#define SERPID      0
#define SERCMD      1
#define SIZE_LIT    2
#define SIZE_BIG    3
#define COUNT_LIT   4
#define COUNT_BIG   5
#define HEADDATA    6


//#define debug

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{

    canIndex = 0;
    passIndex = 0;
    for(uint pass = 1;pass < 2;pass++ )
    {
        QList<uint> nodeList;
        for(uint canId = 1;canId < 128;canId++)
        {
            if(db.getNodeNum(pass,canId))
            {
                nodeList.append(canId);
            }
        }
        nodeMap.insert(pass,nodeList);
    }

    passCount = nodeMap.count();

    m_tcpSocket = new QTcpSocket;
    m_tcpServer = new QTcpServer;
    m_tcpServer->listen(QHostAddress::Any,5000);
    connect(m_tcpServer,SIGNAL(newConnection()),this,SLOT(slotNewConnection()));
}



//接收主机命令
void TcpServer::slotReceiveData()
{

    QByteArray byteArray = m_tcpSocket->readAll();
    if(byteArray[0] == (char)0x0A && byteArray[1] == (char)0x01)
    {
        //节点状态
        moduleStatus();
    }
}

void TcpServer::slotNewConnection()
{
    qDebug()<<"*********slotNewConnection***********";
    canIndex = 0;
    passIndex = 0;
    m_tcpSocket = m_tcpServer->nextPendingConnection();
    connect(m_tcpSocket,SIGNAL(readyRead()),this,SLOT(slotReceiveData()),Qt::DirectConnection);
    connect(m_tcpSocket,SIGNAL(disconnected()),this,SLOT(slotDisConnect()),Qt::DirectConnection);
}

void TcpServer::moduleStatus()
{

    //qDebug()<<"count =============================================: "<<nodeMap.count();
    //当前网络的节点个数
    uint canIdCount = nodeMap.value(1).count();
    if(canIndex == canIdCount)
    {
        canIndex = 0;
        //passIndex++;
//        if(passIndex == passCount)
//        {
//            passIndex = 0;
//        }
    }
    uint pass = 1;
    uint canId  = nodeMap.value(1).value(canIndex);


    if(g_mod[pass][canId].used == false)
    {
        return;
    }

    uchar modeSts = 0;
    if(g_mod[pass][canId].normalFlag == TRUE)
    {
        modeSts = 0;
    }
    if(g_mod[pass][canId].overCurrentFlag == TRUE)
    {
        modeSts = 1;
    }
    if(g_mod[pass][canId].phaseLossFlag == TRUE)
    {
        modeSts = 2;
    }
    if(g_mod[pass][canId].overVoltageFlag == TRUE)
    {
        modeSts = 3;
    }
    if(g_mod[pass][canId].underVoltageFlag == TRUE)
    {
        modeSts = 4;
    }
    if(g_mod[pass][canId].dropFlag == TRUE)
    {
        modeSts = 5;
    }
    if(g_mod[pass][canId].interruptionFlag == TRUE)
    {
        modeSts = 6;
    }

    int type = g_mod[pass][canId].type;



    uint av_1 = g_mod[pass][canId].AV_1;
    uint bv_1 = g_mod[pass][canId].BV_2;
    uint cv_1 = g_mod[pass][canId].CV_3;
    uint av_2 = g_mod[pass][canId].AV_4;
    uint bv_2 = g_mod[pass][canId].BV_5;
    uint cv_2 = g_mod[pass][canId].CV_6;
    uint ai_1 = g_mod[pass][canId].AI_1;
    uint bi_1 = g_mod[pass][canId].BI_2;
    uint ci_1 = g_mod[pass][canId].CI_3;

    sendToClient(pass,canId,type,modeSts,av_1,bv_1,cv_1,av_2,bv_2,cv_2,ai_1,bi_1,ci_1);



    canIndex++;
    qDebug()<<"nodeType : "<<g_mod[pass][canId].type;
    qDebug()<<"passIndex: "<<passIndex;
    qDebug()<<"canIndex : "<<canIndex;
    qDebug()<<"modeSts  : "<<modeSts;
    qDebug()<<"************************";
}

void TcpServer::slotDisConnect()
{
    qDebug()<<"disconnect";
}

void TcpServer::sendToClient(uint pass, uint id, uint type, uint sts, uint av_1, uint bv_1, uint cv_1,
                             uint av_2, uint bv_2, uint cv_2, uint ai_1, uint bi_1, uint ci_1)
{
    QByteArray byteArray;
    byteArray[HEAD_AA] = 0xAA;
    byteArray[HEAD_00] = 0x00;
    byteArray[PASS]    = pass;
    byteArray[CANID]   = id;
    byteArray[TYPE]    = type;
    byteArray[STS]     = sts;
    byteArray[_AV_1]   = av_1;
    byteArray[_BV_1]   = bv_1;
    byteArray[_CV_1]   = cv_1;
    byteArray[_AV_2]   = av_2;
    byteArray[_BV_2]   = bv_2;
    byteArray[_CV_2]   = cv_2;
    byteArray[_AI_1]   = ai_1;
    byteArray[_BI_1]   = bi_1;
    byteArray[_CI_1]   = ci_1;
    uchar crc = 0;
    for(int i = 2;i < CRC;i++)
    {
        crc += byteArray[i];
    }
    byteArray[CRC] = crc;
    m_tcpSocket->write(byteArray);
}

