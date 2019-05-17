#ifndef MOUDLE_H
#define MOUDLE_H
#include <QObject>

class Moudle
{
public:
    Moudle();
    bool used;    //是否存在
    uint type;
    uint net;
    uint id;
    bool dropFlag;
    bool normalFlag;        //
    bool phaseLossFlag;     //错相
    bool overCurrentFlag;   //过流
    bool overVoltageFlag;   //过压
    bool underVoltageFlag;  //欠压
    bool interruptionFlag;  //供电中断
    bool insertDrop;
    bool insertNormal;
    bool insertphaseLoss;     //错相
    bool insertOverCurrent;   //过流
    bool insertOverVoltage;   //过压
    bool insertUnderVoltage;  //欠压
    bool insertInterruption;  //供电中断
    uint AV_1;
    uint BV_2;
    uint CV_3;
    uint AV_4;
    uint BV_5;
    uint CV_6;
    qreal AI_1;
    qreal BI_2;
    qreal CI_3;
    uint IR;
    uint package;
    uint dropTimes;
    uint leakTimes;
    uint tempTimes;
};

#endif // MOUDLE_H
