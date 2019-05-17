#ifndef GLOBALDATA_H
#define GLOBALDATA_H
#include "Moudle/moudle.h"
#include "MyPrint/myprint.h"
#define PASSNUM 2
#define NODENUM 128
#define ALLID 0
#define W_TIME 150
#define R_TIME 20
#define LOSTTIMES 4
#define CMD_SE_STATE    0X01//读取探测器状态
#define CMD_SE_RESET    0X05//复位探测器

#define CMD_RE_STATE    0X02//探测器回复状态
#define CMD_RE_RESET    0X06//探测器回复复位

#define MOD_NONE    1//无类型
#define MOD_V3      2//双路三相电压型
#define MOD_V       3//六路单相电压型
#define MOD_VA3     4//电压电流型
#define MOD_DVA     5//直流电压电流型
#define MOD_VA      6//单项电压电流
#define MOD_VN3     7//三项双路有零
#define MOD_VAN3    8//电压电流有零
#define MOD_2VAN3   9//两路三项电压一路三项电流

#define NORMAL          0x00//正常
#define OVERCURRENT     0x01//过流
#define PHASELOSS       0x02//错相
#define OVERVOLTAGE     0x03//过压
#define UNDERVOLTAGE    0x04//欠压
#define CANERROR        0x06//通讯中断
#define INTERRUPTION    0x05//供电中断
#define POWERBREAK      0x17//主电断开
#define BPOWERBREAK     0x18//备电断开
#define BPOWERSHORT     0x19//备电短路


#define NODEBASE 40



#define S_NORMAL 0x00//正常
#define S_ERROR  0x02//故障


#define ALLDATA    0
#define ALARMDATA  1
#define ERRORDATA  2

#define MASTER     1 //主机
#define TIMES      5

#define INTERDATA     1
#define ERRORDATA     2

extern Moudle g_mod[3][129];
extern bool g_login;
extern bool g_lock;

class GlobalData
{
public:
    GlobalData();
};

#endif // GLOBALDATA_H
