#include "manager.h"

Manager::Manager(QObject *parent) : QObject(parent)
{
    initVar();
    initConnect();
}

Manager::~Manager()
{
    qDebug()<<"~Manager";
}

void Manager::initVar()
{
    m_times = 0;
    m_record    = new Record;
    m_hostInfo  = new HostInfo;
    m_userLogin = new UserLogin;
    m_systemSet = new SystemSet;
    m_mainPage  = new MainWindow;
    m_devInfo   = new DevInfo;
    m_gpio      = new GpioControl;
    m_screenCheck =  new ScreenCheck;




    //m_db.delData("delete from NODE;");
    //for(int i = 1;i< 129;i++)
    //{
    //    QString add = QString::number(1)+"."+QString::number(i);
    //    m_db.insertNode(1,i,add);
    //}
    //for(int i = 1;i< 129;i++)
    //{
    //    QString add = QString::number(2)+"."+QString::number(i);
    //    m_db.insertNode(2,i,add);
    //}

    //数据库有多少个点开启就现实多少;



    //初始化CANPort
    m_passNum = m_db.getSerialNumber().mid(2,1).toInt();
    if(m_passNum == 1)
    {
        canMoudle_1 = new CanMoudle;
        canMoudle_1->initCanMoudle(1,"can0");
        connect(canMoudle_1,SIGNAL(sigCanLed()),m_gpio,SLOT(slotCanLed()));
        connect(canMoudle_1,SIGNAL(sigBtnSound()),m_devInfo,SLOT(slotSound()));
        connect(canMoudle_1,SIGNAL(sigSoundType(int)),m_gpio,SLOT(slotControlSound(int)));
        canMoudle_1->start();
    }
    else
    {
        canMoudle_1 = new CanMoudle;
        canMoudle_1->initCanMoudle(1,"can0");
        connect(canMoudle_1,SIGNAL(sigCanLed()),m_gpio,SLOT(slotCanLed()));
        connect(canMoudle_1,SIGNAL(sigBtnSound()),m_devInfo,SLOT(slotSound()));
        connect(canMoudle_1,SIGNAL(sigSoundType(int)),m_gpio,SLOT(slotControlSound(int)));
        canMoudle_1->start();

        canMoudle_2 = new CanMoudle;
        canMoudle_2->initCanMoudle(2,"can1");
        connect(canMoudle_2,SIGNAL(sigCanLed()),m_gpio,SLOT(slotCanLed()));
        connect(canMoudle_2,SIGNAL(sigBtnSound()),m_devInfo,SLOT(slotSound()));
        connect(canMoudle_2,SIGNAL(sigSoundType(int)),m_gpio,SLOT(slotControlSound(int)));
        canMoudle_2->start();

    }

}

void Manager::initConnect()
{
    //主页按钮
    connect(m_mainPage,SIGNAL(sigBtnRecord()),m_record,SLOT(slotRecordShow()));
    connect(m_mainPage,SIGNAL(sigBtnUser()),m_userLogin,SLOT(slotLoginShow()));
    connect(m_mainPage,SIGNAL(sigBtnDevInfo()),m_devInfo,SLOT(slotDevInfoShow()));
    connect(m_mainPage,SIGNAL(sigBtnHostSet()),m_systemSet,SLOT(slotSystemShow()));
    connect(m_mainPage,SIGNAL(sigBtnHostInfo()),m_hostInfo,SLOT(slotHostInfoShow()));
    connect(m_mainPage,SIGNAL(sigBtnHostCheck()),this,SLOT(slotBtnSelfCheck()));
    connect(m_mainPage,SIGNAL(sigLoginStatus(int)),m_userLogin,SIGNAL(sigLoginStatus(int)));

    connect(m_userLogin,SIGNAL(sigLoginStatus(int)),m_mainPage,SLOT(slotLoginStatus(int)));
    connect(m_userLogin,SIGNAL(sigLoginStatus(int)),m_devInfo ,SLOT(slotLoginStatus(int)));
    connect(m_userLogin,SIGNAL(sigLoginStatus(int)),m_systemSet ,SLOT(slotLoginStatus(int)));
    connect(m_userLogin,SIGNAL(sigLoginStatus(int)),m_record,SLOT(slotLoginStatus(int)));
    connect(m_userLogin,SIGNAL(sigMainPageShow()),m_mainPage,SLOT(show()));

    connect(m_gpio,SIGNAL(sigBtnSound()),m_devInfo,SLOT(slotSound()));
    connect(m_devInfo,SIGNAL(sigSoundType(int)),m_gpio,SLOT(slotControlSound(int)));
    connect(m_devInfo,SIGNAL(sigErrorLed(bool)),m_gpio,SLOT(slotErrorControl(bool)));
    connect(m_gpio,SIGNAL(sigPowerSts(bool)),m_devInfo,SLOT(slotPowerSts(bool)));
    m_selfTimer = new QTimer;
    connect(m_selfTimer,SIGNAL(timeout()),this,SLOT(slotSelfCheckTimer()));


    //connect(m_hostInfo,SIGNAL(sigMainPageShow()),m_mainPage,SLOT(show()));
    connect(m_devInfo,SIGNAL(sigMainPageShow()),m_mainPage,SLOT(show()));


}

void Manager::managerWork()
{
    QString ip = m_db.getHost();
    //qDebug()<<"================"+ip;
    QProcess process;//ifconfig eth0 192.168.126.245
    QString cmd = QString("ifconfig eth0 ")+ip;
    //QString cmd = QStringLiteral("ifconfig eth0 ")+ip;//jiang
    process.execute(cmd);//jiang


    //jiangstart20190512
    QProcess process1;
    QString cmd1 = QString("route add default gw 192.168.0.1");
    process1.execute(cmd1);//jiang
//    QProcess process2;
//    QString cmd2 = QString("echo nameserver 219.141.136.10 >> /etc/resolv.conf");
//    process2.execute(cmd2);//jiang
    //jiangend20190512

    //QString m_mac = m_db.getMACAdress();
    //m_db.setLocalMacToDB(m_mac);


    m_udpClient = new UdpClient;//jiang20190512

    //qDebug()<<"getKeyTimeOut = "<<m_db.getKeyTimeOut();
    if(m_db.getKeyTimeOut() == true)
    {
        m_userLogin->showLock();
    }
    else
    {
        m_mainPage->show();
    }
}


void Manager::slotBtnSelfCheck()
{
    if(m_passNum == 1)
    {
        canMoudle_1->pause.lock();
    }
    else
    {
        canMoudle_1->pause.lock();
        canMoudle_2->pause.lock();
    }

    m_screenCheck->show();
    m_gpio->defaultStatus();
    m_selfTimer->start(1000);

}

void Manager::slotSelfCheckTimer()
{
    m_times++;
    if(m_times < 6)
    {
        m_screenCheck->setColor(m_times);
    }
    else
    {
        m_screenCheck->setColor(m_times-5);
    }

    m_gpio->selfCheck(m_times);

    if(m_times == 11)
    {
        m_times = 0;
        m_selfTimer->stop();
        if(m_passNum == 1)
        {
            canMoudle_1->pause.unlock();
        }
        else
        {
            canMoudle_1->pause.unlock();
            canMoudle_2->pause.unlock();
        }
        m_screenCheck->hide();
    }
}

