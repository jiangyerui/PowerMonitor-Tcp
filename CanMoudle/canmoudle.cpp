#include "canmoudle.h"
#include "time.h"
CanMoudle::CanMoudle()
{

    m_canCmd = 0;
    m_soundSW = true;
    m_readTimes = 0;
    m_calTimes = 0;
    m_inter   = 0;
    m_phase   = 0;
    m_droped  = 0;
    m_overVol = 0;
    m_overCur = 0;
    m_underVol= 0;

    m_dropNum[0] = 0;
    m_dropNum[1] = 0;
    m_dropNum[2] = 0;
    m_dropTimes = 0;

}

void CanMoudle::initCanMoudle(int net, const char *canPort)
{
    m_canfd = initCanPort(canPort);//初始化端口
    m_net = net;

    for(uint id = 1;id <= NODENUM;id++)
    {
        if(m_db.getNodeNum(net,id))
        {
            addCanCmd(id,0x01,8);
            g_mod[m_net][id].id   = id;
            g_mod[m_net][id].net  = net;
            g_mod[m_net][id].used = true;
            g_mod[m_net][id].normalFlag = true;
            g_mod[m_net][id].IR = m_db.getModRatio(m_net,id);
        }
    }
}

void CanMoudle::run()
{
    //写命令
    struct can_frame canWriteFrame;
    //读数据
    struct can_frame canReadFrame;
    while(1){
        pause.lock();
        msleep(W_TIME);
        if(m_canCmdList.count() != 0)
        {
            if(m_canCmd == m_canCmdList.count())
            {
                m_canCmd = 0;
            }
            canWriteFrame = m_canCmdList.at(m_canCmd);
            canWrite(m_canfd,canWriteFrame);

            int ret = 0;

            for(int time = 0;time < 20;time++)
            {
                msleep(1);
                ret = canRead(m_canfd,canReadFrame);
                if(ret > 0 && m_canCmdList.at(m_canCmd).can_id == canReadFrame.can_id)
                {
                    break;
                }
            }


            if(ret > 0)//收到数据
            {
                if(m_canCmdList.at(m_canCmd).can_id == canReadFrame.can_id)
                {
                    emit sigCanLed();
                    dealCanData(canReadFrame);
                    modStatus(m_canCmdList.at(m_canCmd).can_id);
                    m_canCmd++;     //发送下一条
                    m_readTimes = 0;
                }
                m_dropNum[m_net] = 0;
            }
            else
            {
                //未收到数据
                m_readTimes++;
                if(LOSTTIMES == m_readTimes)
                {
                    g_mod[m_net][m_canCmdList.at(m_canCmd).can_id].dropFlag = true;
                    g_mod[m_net][m_canCmdList.at(m_canCmd).can_id].normalFlag = false;
                    modStatus(m_canCmdList.at(m_canCmd).can_id);
                    m_canCmd++;
                    m_readTimes = 0;
                }
            }

            //检测通讯短路故障
            if(m_dropNum[m_net] == m_canCmdList.count() && m_canCmdList.count() != 0)
            {
                m_dropTimes++;
                if(m_dropTimes == 100)
                {
                    m_dropTimes = 0;
                    ::system("source regCan.sh");
                }
            }
            pause.unlock();
        }
    }
}

int CanMoudle::initCanPort(const char *canName)
{
    struct ifreq ifr;
    struct sockaddr_can addr;
    int canfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(canfd == -1)
    {
        qDebug("socket error");
    }
    else
    {
        strcpy(ifr.ifr_name, canName );
        if(ioctl(canfd, SIOCGIFINDEX, &ifr))
        {
            qDebug()<<"ioctl error";
            return -1;
        }

        addr.can_family = PF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if(bind(canfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            qDebug()<<"bind error";
            return -1;
        }
        qDebug()<<"**** Open "<<canName<<" OK **** ";
    }
    return canfd;
}

void CanMoudle::modStatus(int id)
{
    if(g_mod[m_net][id].used == true)
    {
        Moudle mod   = g_mod[m_net][id];

        QString add  = m_db.getNodeAddress(m_net,mod.id);
        uint curTime = QDateTime::currentDateTime().toTime_t();
        QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd/hh:mm:ss");
        if(g_mod[m_net][id].dropFlag == true)
        {
            if(g_mod[m_net][id].insertDrop == false)
            {
                g_mod[m_net][id].insertDrop = true;
                m_dropNum[m_net]++;
                emit sigBtnSound();
                emit sigSoundType(S_ERROR);
                m_db.insertTemp(mod.net,mod.id,CANERROR,curTime);
                m_db.insertAlarm(mod.net,mod.id,CANERROR,curTime,add);
                if(m_db.getPrintStyle())
                {
                    emit sigConnectPrint(QString::number(m_net),QString::number(id),tr("通讯故障"),currentTime,add);
//                    qDebug()<<"CANERROR";
                }

                m_db.delTemp(m_net,mod.id,PHASELOSS);
                m_db.delTemp(m_net,mod.id,OVERVOLTAGE);
                m_db.delTemp(m_net,mod.id,OVERCURRENT);
                m_db.delTemp(m_net,mod.id,UNDERVOLTAGE);
                m_db.delTemp(m_net,mod.id,INTERRUPTION);

                g_mod[m_net][mod.id].insertphaseLoss    = false;
                g_mod[m_net][mod.id].insertOverCurrent  = false;
                g_mod[m_net][mod.id].insertOverVoltage  = false;
                g_mod[m_net][mod.id].insertUnderVoltage = false;
                g_mod[m_net][mod.id].insertInterruption = false;


                g_mod[m_net][mod.id].AV_1 = 0;
                g_mod[m_net][mod.id].BV_2 = 0;
                g_mod[m_net][mod.id].CV_3 = 0;

                g_mod[m_net][mod.id].AV_4 = 0;
                g_mod[m_net][mod.id].BV_5 = 0;
                g_mod[m_net][mod.id].CV_6 = 0;

                g_mod[m_net][mod.id].AI_1 = 0;
                g_mod[m_net][mod.id].BI_2 = 0;
                g_mod[m_net][mod.id].CI_3 = 0;

            }
        }
        else if(g_mod[m_net][id].overCurrentFlag  == true)
        {
            if(g_mod[m_net][id].insertOverCurrent == false)
            {
                g_mod[m_net][id].insertOverCurrent = true;
                emit sigBtnSound();
                emit sigSoundType(S_ERROR);
                m_db.insertTemp(mod.net,mod.id,OVERCURRENT,curTime);
                m_db.insertAlarm(mod.net,mod.id,OVERCURRENT,curTime,add);

                if(m_db.getPrintStyle())
                {
                    emit sigConnectPrint(QString::number(m_net),QString::number(id),tr("过流"),currentTime,add);
                }

                m_db.delTemp(m_net,mod.id,CANERROR);
                m_db.delTemp(m_net,mod.id,PHASELOSS);
                m_db.delTemp(m_net,mod.id,OVERVOLTAGE);
                m_db.delTemp(m_net,mod.id,UNDERVOLTAGE);
                m_db.delTemp(m_net,mod.id,INTERRUPTION);
            }

        }
        else if(g_mod[m_net][id].phaseLossFlag    == true)
        {
            if(g_mod[m_net][id].insertphaseLoss == false)
            {
                g_mod[m_net][id].insertphaseLoss = true;
                emit sigBtnSound();
                emit sigSoundType(S_ERROR);
                m_db.insertTemp(mod.net,mod.id,PHASELOSS,curTime);
                m_db.insertAlarm(mod.net,mod.id,PHASELOSS,curTime,add);

                m_db.delTemp(m_net,mod.id,CANERROR);
                m_db.delTemp(m_net,mod.id,OVERVOLTAGE);
                m_db.delTemp(m_net,mod.id,OVERCURRENT);
                m_db.delTemp(m_net,mod.id,UNDERVOLTAGE);
                m_db.delTemp(m_net,mod.id,INTERRUPTION);
                if(m_db.getPrintStyle())
                {
                    emit sigConnectPrint(QString::number(m_net),QString::number(id),tr("错相"),currentTime,add);
                }
            }
        }
        else if(g_mod[m_net][id].overVoltageFlag  == true)
        {
            if(g_mod[m_net][id].insertOverVoltage == false)
            {
                g_mod[m_net][id].insertOverVoltage = true;
                emit sigBtnSound();
                emit sigSoundType(S_ERROR);
                m_db.insertTemp(mod.net,mod.id,OVERVOLTAGE,curTime);
                m_db.insertAlarm(mod.net,mod.id,OVERVOLTAGE,curTime,add);

                m_db.delTemp(m_net,mod.id,CANERROR);
                m_db.delTemp(m_net,mod.id,PHASELOSS);
                m_db.delTemp(m_net,mod.id,OVERCURRENT);
                m_db.delTemp(m_net,mod.id,UNDERVOLTAGE);
                m_db.delTemp(m_net,mod.id,INTERRUPTION);
                if(m_db.getPrintStyle())
                {
                    emit sigConnectPrint(QString::number(m_net),QString::number(id),tr("过压"),currentTime,add);
                }
            }
        }
        else if(g_mod[m_net][id].underVoltageFlag == true)
        {
            if(g_mod[m_net][id].insertUnderVoltage == false)
            {
                g_mod[m_net][id].insertUnderVoltage = true;
                emit sigBtnSound();
                emit sigSoundType(S_ERROR);
                m_db.insertTemp(mod.net,mod.id,UNDERVOLTAGE,curTime);
                m_db.insertAlarm(mod.net,mod.id,UNDERVOLTAGE,curTime,add);

                m_db.delTemp(m_net,mod.id,CANERROR);
                m_db.delTemp(m_net,mod.id,PHASELOSS);
                m_db.delTemp(m_net,mod.id,OVERVOLTAGE);
                m_db.delTemp(m_net,mod.id,OVERCURRENT);
                m_db.delTemp(m_net,mod.id,INTERRUPTION);
                if(m_db.getPrintStyle())
                {
                    emit sigConnectPrint(QString::number(m_net),QString::number(id),tr("欠压"),currentTime,add);
                }
            }
        }
        else if(g_mod[m_net][id].interruptionFlag == true)
        {
            if(g_mod[m_net][id].insertInterruption == false)
            {
                g_mod[m_net][id].insertInterruption = true;
                emit sigBtnSound();
                emit sigSoundType(S_ERROR);
                m_db.insertTemp(mod.net,mod.id,INTERRUPTION,curTime);
                m_db.insertAlarm(mod.net,mod.id,INTERRUPTION,curTime,add);

                m_db.delTemp(m_net,mod.id,CANERROR);
                m_db.delTemp(m_net,mod.id,PHASELOSS);
                m_db.delTemp(m_net,mod.id,OVERVOLTAGE);
                m_db.delTemp(m_net,mod.id,OVERCURRENT);
                m_db.delTemp(m_net,mod.id,UNDERVOLTAGE);
                if(m_db.getPrintStyle())
                {
                    emit sigConnectPrint(QString::number(m_net),QString::number(id),tr("电源中断"),currentTime,add);
                }

                g_mod[m_net][mod.id].AV_1 = 0;
                g_mod[m_net][mod.id].BV_2 = 0;
                g_mod[m_net][mod.id].CV_3 = 0;

                g_mod[m_net][mod.id].AV_4 = 0;
                g_mod[m_net][mod.id].BV_5 = 0;
                g_mod[m_net][mod.id].CV_6 = 0;

                g_mod[m_net][mod.id].AI_1 = 0;
                g_mod[m_net][mod.id].BI_2 = 0;
                g_mod[m_net][mod.id].CI_3 = 0;
            }
        }
        else
        {
            m_db.delTemp(m_net,mod.id,CANERROR);
            m_db.delTemp(m_net,mod.id,PHASELOSS);
            m_db.delTemp(m_net,mod.id,OVERVOLTAGE);
            m_db.delTemp(m_net,mod.id,OVERCURRENT);
            m_db.delTemp(m_net,mod.id,UNDERVOLTAGE);
            m_db.delTemp(m_net,mod.id,INTERRUPTION);
        }

    }
}

void CanMoudle::addCanCmd(uint id, uint cmd,uchar lon)
{
    can_frame canFrame;
    canFrame.can_dlc = lon;
    canFrame.can_id  = id;
    canFrame.data[0] = cmd;
    for(int i = 1;i<lon;i++)
    {
        canFrame.data[i] = 0;
    }
    m_canCmdList.append(canFrame);
}

//#define DEBUG

void CanMoudle::dealCanData(can_frame frame)
{
    uint canId;
    qreal _AI;  qreal _ai;
    qreal _BI;  qreal _bi;
    qreal _CI;  qreal _ci;

    canId = frame.can_id;
    g_mod[m_net][canId].id = canId;
    g_mod[m_net][canId].net = m_net;
    g_mod[m_net][canId].insertDrop = false;

#ifdef DEBUG
    qDebug()<<"******canReadFrame******";
    qDebug()<<"can_id  : "<<frame.can_id;
    qDebug()<<"can_dlc : "<<frame.can_dlc;
    qDebug()<<" data   : "<<frame.data[0];
    qDebug()<<" data   : "<<frame.data[1];
    qDebug()<<" data   : "<<frame.data[2];
    qDebug()<<" data   : "<<frame.data[3];
    qDebug()<<" data   : "<<frame.data[4];
    qDebug()<<" data   : "<<frame.data[5];
    qDebug()<<" data   : "<<frame.data[6];
    qDebug()<<" data   : "<<frame.data[7];
#endif
    //接收处理数据
    g_mod[m_net][canId].type = frame.data[0];
    switch (g_mod[m_net][canId].type) {//modType
    case MOD_V:
    case MOD_V3://三项双路无零
    case MOD_VN3://三项双路有零

        g_mod[m_net][canId].AV_1 = frame.data[2] * 2;
        g_mod[m_net][canId].BV_2 = frame.data[3] * 2;
        g_mod[m_net][canId].CV_3 = frame.data[4] * 2;
        g_mod[m_net][canId].AV_4 = frame.data[5] * 2;
        g_mod[m_net][canId].BV_5 = frame.data[6] * 2;
        g_mod[m_net][canId].CV_6 = frame.data[7] * 2;

        break;
    case MOD_VA:
    case MOD_VA3: //电压电流无零
    case MOD_VAN3://电压电流有零

        g_mod[m_net][canId].AV_1 = frame.data[2] * 2;
        g_mod[m_net][canId].BV_2 = frame.data[3] * 2;
        g_mod[m_net][canId].CV_3 = frame.data[4] * 2;

        _AI = frame.data[5] >> 4;   _ai = frame.data[5] & 0x0F;
        _BI = frame.data[6] >> 4;   _bi = frame.data[6] & 0x0F;
        _CI = frame.data[7] >> 4;   _ci = frame.data[7] & 0x0F;

        g_mod[m_net][canId].AI_1 = (_AI + _ai/10);
        g_mod[m_net][canId].BI_2 = (_BI + _bi/10);
        g_mod[m_net][canId].CI_3 = (_CI + _ci/10);

        break;
    case MOD_2VAN3://两路三项电压一路三项电流

        if(frame.can_dlc == 8)
        {
            g_mod[m_net][canId].AV_1 = frame.data[2] * 2;
            g_mod[m_net][canId].BV_2 = frame.data[3] * 2;
            g_mod[m_net][canId].CV_3 = frame.data[4] * 2;
            g_mod[m_net][canId].AV_4 = frame.data[5] * 2;
            g_mod[m_net][canId].BV_5 = frame.data[6] * 2;
            g_mod[m_net][canId].CV_6 = frame.data[7] * 2;
        }

        can_frame canFrame;

        for(int i = 0;i<100;i++)
        {
            usleep(60);
            int ret = canRead(m_canfd,canFrame);
            if(ret > 0)
            {
                if(canFrame.can_dlc == 7)
                {
                    _AI = canFrame.data[4] >> 4;   _ai = canFrame.data[4] & 0x0F;
                    _BI = canFrame.data[5] >> 4;   _bi = canFrame.data[5] & 0x0F;
                    _CI = canFrame.data[6] >> 4;   _ci = canFrame.data[6] & 0x0F;

                    g_mod[m_net][canId].AI_1 = (_AI + _ai/10);
                    g_mod[m_net][canId].BI_2 = (_BI + _bi/10);
                    g_mod[m_net][canId].CI_3 = (_CI + _ci/10);
                }
                break;
            }
        }
        break;
    default:
        break;
    }
    //更新数据
    switch (frame.data[1]) {

    case NORMAL:
        g_mod[m_net][canId].dropFlag         = false;
        g_mod[m_net][canId].normalFlag       = true;
        g_mod[m_net][canId].phaseLossFlag    = false;
        g_mod[m_net][canId].overCurrentFlag  = false;
        g_mod[m_net][canId].overVoltageFlag  = false;
        g_mod[m_net][canId].underVoltageFlag = false;
        g_mod[m_net][canId].interruptionFlag = false;

        g_mod[m_net][canId].insertDrop         = false;
        g_mod[m_net][canId].insertphaseLoss    = false;
        g_mod[m_net][canId].insertOverCurrent  = false;
        g_mod[m_net][canId].insertOverVoltage  = false;
        g_mod[m_net][canId].insertUnderVoltage = false;
        g_mod[m_net][canId].insertInterruption = false;

        break;
    case PHASELOSS:
        g_mod[m_net][canId].dropFlag         = false;
        g_mod[m_net][canId].normalFlag       = false;
        g_mod[m_net][canId].phaseLossFlag    = true;
        g_mod[m_net][canId].overCurrentFlag  = false;
        g_mod[m_net][canId].overVoltageFlag  = false;
        g_mod[m_net][canId].underVoltageFlag = false;
        g_mod[m_net][canId].interruptionFlag = false;

        g_mod[m_net][canId].insertDrop         = false;
        g_mod[m_net][canId].insertOverCurrent  = false;
        g_mod[m_net][canId].insertOverVoltage  = false;
        g_mod[m_net][canId].insertUnderVoltage = false;
        g_mod[m_net][canId].insertInterruption = false;

        break;
    case OVERCURRENT:
        g_mod[m_net][canId].dropFlag         = false;
        g_mod[m_net][canId].normalFlag       = false;
        g_mod[m_net][canId].phaseLossFlag    = false;
        g_mod[m_net][canId].overCurrentFlag  = true;
        g_mod[m_net][canId].overVoltageFlag  = false;
        g_mod[m_net][canId].underVoltageFlag = false;
        g_mod[m_net][canId].interruptionFlag = false;

        g_mod[m_net][canId].insertDrop         = false;
        g_mod[m_net][canId].insertphaseLoss    = false;
        g_mod[m_net][canId].insertOverVoltage  = false;
        g_mod[m_net][canId].insertUnderVoltage = false;
        g_mod[m_net][canId].insertInterruption = false;

        break;
    case OVERVOLTAGE:
        g_mod[m_net][canId].dropFlag         = false;
        g_mod[m_net][canId].normalFlag       = false;
        g_mod[m_net][canId].phaseLossFlag    = false;
        g_mod[m_net][canId].overCurrentFlag  = false;
        g_mod[m_net][canId].overVoltageFlag  = true;
        g_mod[m_net][canId].underVoltageFlag = false;
        g_mod[m_net][canId].interruptionFlag = false;

        g_mod[m_net][canId].insertDrop         = false;
        g_mod[m_net][canId].insertphaseLoss    = false;
        g_mod[m_net][canId].insertOverCurrent  = false;
        g_mod[m_net][canId].insertUnderVoltage = false;
        g_mod[m_net][canId].insertInterruption = false;

        break;
    case UNDERVOLTAGE:
        g_mod[m_net][canId].dropFlag         = false;
        g_mod[m_net][canId].normalFlag       = false;
        g_mod[m_net][canId].phaseLossFlag    = false;
        g_mod[m_net][canId].overCurrentFlag  = false;
        g_mod[m_net][canId].overVoltageFlag  = false;
        g_mod[m_net][canId].underVoltageFlag = true;
        g_mod[m_net][canId].interruptionFlag = false;

        g_mod[m_net][canId].insertDrop         = false;
        g_mod[m_net][canId].insertphaseLoss    = false;
        g_mod[m_net][canId].insertOverCurrent  = false;
        g_mod[m_net][canId].insertOverVoltage  = false;
        g_mod[m_net][canId].insertInterruption = false;

        break;
    case INTERRUPTION:
        g_mod[m_net][canId].dropFlag         = false;
        g_mod[m_net][canId].normalFlag       = false;
        g_mod[m_net][canId].phaseLossFlag    = false;
        g_mod[m_net][canId].overCurrentFlag  = false;
        g_mod[m_net][canId].overVoltageFlag  = false;
        g_mod[m_net][canId].underVoltageFlag = false;
        g_mod[m_net][canId].interruptionFlag = true;

        g_mod[m_net][canId].insertDrop         = false;
        g_mod[m_net][canId].insertphaseLoss    = false;
        g_mod[m_net][canId].insertOverCurrent  = false;
        g_mod[m_net][canId].insertOverVoltage  = false;
        g_mod[m_net][canId].insertUnderVoltage = false;

        break;
    default:
        break;
    }


}

uint CanMoudle::getCmdNum()
{
    int cmdNum;
    int node = m_db.getSerialNumber().mid(2,1).toUInt();

    if(node <= 0)
    {
        cmdNum = NODEBASE;
    }
    else
    {
        cmdNum = NODEBASE * node;
    }

    return cmdNum;
}

void CanMoudle::canWrite(int canfd, can_frame &frame)
{
//    qDebug()<<"******canWrite******";
//    qDebug()<<"can_id  : "<<frame.can_id;
//    qDebug()<<"can_dlc : "<<frame.can_dlc;
//    qDebug()<<" data   : "<<frame.data[0];
//    qDebug()<<" data   : "<<frame.data[1];
//    qDebug()<<" data   : "<<frame.data[2];
//    qDebug()<<" data   : "<<frame.data[3];
//    qDebug()<<" data   : "<<frame.data[4];
//    qDebug()<<" data   : "<<frame.data[5];
//    qDebug()<<" data   : "<<frame.data[6];
//    qDebug()<<" data   : "<<frame.data[7];

    send(canfd, &frame, sizeof(struct can_frame),MSG_DONTWAIT);

}

int CanMoudle::canRead(int canfd, can_frame &frame)
{
    int ret = recv(canfd, &frame, sizeof(struct can_frame),MSG_DONTWAIT);
    return ret;
}

void CanMoudle::canReadDontWait(int canfd, can_frame &frame)
{
    recv(canfd, &frame, sizeof(struct can_frame),MSG_DONTWAIT);
}

void CanMoudle::slotSetSoundSW(bool soundSW)
{
    m_soundSW = soundSW;
}


