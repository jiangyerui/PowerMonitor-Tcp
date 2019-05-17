#ifndef CANMOUDLE_H
#define CANMOUDLE_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <sys/socket.h>
#include <linux/types.h>
#include "Moudle/moudle.h"
#include "MySqlite/mysqlite.h"
#include "GlobalData/globaldata.h"




class CanMoudle : public QThread
{
    Q_OBJECT
public:
    explicit CanMoudle();
    QMutex pause;
    void initCanMoudle(int net, const char *canPort);
    void modStatus(int id);
private:
    int m_net;
    int m_canfd;
    int m_canCmd;
    int m_calTimes;
    int m_readTimes;


    int m_droped  ;
    int m_overCur ;
    int m_overVol ;
    int m_underVol;
    int m_phase   ;
    int m_inter   ;
    bool m_soundSW;
    int m_dropNum[3];
    int m_dropTimes;


    MySqlite m_db;
    QList<can_frame> m_canCmdList;

    int  initCanPort(const char *canName);
    uint getCmdNum();
    void canWrite(int canfd, can_frame &frame); //向CAN口写入数据
    int canRead(int canfd, can_frame &frame);  //从CAN口读入数据-阻塞等待
    void canReadDontWait(int canfd, can_frame &frame);  //从CAN口读入数据-不阻塞
    void addCanCmd(uint id, uint cmd, uchar lon);
    void dealCanData(can_frame frame);

protected:
    void run();


signals:
    void sigBtnSound();
    void sigCanLed();
    void sigUpdateModMap(QMap<int ,QList<Moudle> > moudleMap);
    void sigUpdate();
    void sigSoundType(int type);

    void sigConnectPrint(QString,QString,QString,QString,QString);

    void sigSoundControl(int net,int soundType, bool soundSwitch);
    void sigErrorControl(bool enable);
public slots:
    void slotSetSoundSW(bool soundSW);

};

#endif // CANMOUDLE_H
