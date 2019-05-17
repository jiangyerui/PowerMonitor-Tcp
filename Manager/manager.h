#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QDebug>
#include "DevInfo/devinfo.h"
#include "HostInfo/hostinfo.h"
#include "Record/record.h"
#include "DevInfo/devinfo.h"
#include "UserLogin/userlogin.h"
#include "SystemSet/systemset.h"
#include "mainwindow.h"
#include "CanMoudle/canmoudle.h"
#include "GpioControl/gpiocontrol.h"
#include "SelfCheck/screencheck.h"
#include "udpclient.h"
class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    ~Manager();
    void initVar();
    void initConnect();
    void managerWork();


private:
    CanMoudle *canMoudle_2;
    CanMoudle *canMoudle_1;
    DevInfo *m_devInfo;
    HostInfo *m_hostInfo;
    Record *m_record;
    SystemSet *m_systemSet;
    UserLogin *m_userLogin;
    MainWindow *m_mainPage;
    GpioControl *m_gpio;
    ScreenCheck *m_screenCheck;
    MySqlite m_db;

    UdpClient *m_udpClient;

    QTimer *m_selfTimer;
    int m_times;
    int m_passNum;


private slots:

    void slotBtnSelfCheck();
    void slotSelfCheckTimer();

};

#endif // MANAGER_H
