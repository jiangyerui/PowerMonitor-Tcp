#include "moudle.h"

Moudle::Moudle()
{
    used = false;
    type = 7;
    id   = 0;
    net  = 0;
    AV_1 = 220;
    BV_2 = 220;
    CV_3 = 220;
    AV_4 = 220;
    BV_5 = 220;
    CV_6 = 220;
    AI_1 = 10.1;
    BI_2 = 10.2;
    CI_3 = 10.4;
    //IR = 1;

    dropFlag         = false;
    normalFlag       = true;
    phaseLossFlag    = false;
    overCurrentFlag  = false;
    overVoltageFlag  = false;
    underVoltageFlag = false;
    interruptionFlag = false;

    insertDrop         = false;
    insertNormal       = false;
    insertphaseLoss    = false;
    insertOverCurrent  = false;
    insertOverVoltage  = false;
    insertUnderVoltage = false;
    insertInterruption = false;

    package = 0;
    dropTimes = 0;
    leakTimes = 0;
    tempTimes = 0;
}

