#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "watchdog/watchdog.h"
#include <QApplication>
#include "TcpServer/tcpserver.h"

#define GPIODEV   "/dev/gpio"
#define DEVICE    "/dev/watchdog"
#define WatchDog  //看门狗
#define TCP_IP
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0,0,800,480);
    initVar();
    initConnect();
    m_timer = new QTimer;
    connect(m_timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
    m_timer->start(1000);


#ifdef TCP_IP
    //网络接口
    TcpServer *tcpServer = new TcpServer;
    QThread* tcpThread = new QThread;
    tcpServer->moveToThread(tcpThread);
    tcpThread->start();
#endif


#ifdef WatchDog
    //看门狗 参数:char* 设备名称 ,int 溢出时间
    Watchdog::init((char*)DEVICE,TIMEOUT);//jiang
#endif

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initVar()
{
    ui->tBtn_user->setEnabled(true);
    ui->tBtn_devInfo->setEnabled(true);
    ui->tBtn_hostInfo->setEnabled(true);

    ui->tBtn_record->setEnabled(false);
    ui->tBtn_hostSet->setEnabled(false);
    ui->tBtn_hostCheck->setEnabled(false);

}

void MainWindow::initConnect()
{
    connect(ui->tBtn_user,SIGNAL(clicked(bool)),this,SIGNAL(sigBtnUser()));
    connect(ui->tBtn_record,SIGNAL(clicked(bool)),this,SIGNAL(sigBtnRecord()));
    connect(ui->tBtn_hostSet,SIGNAL(clicked(bool)),this,SIGNAL(sigBtnHostSet()));
    connect(ui->tBtn_devInfo,SIGNAL(clicked(bool)),this,SIGNAL(sigBtnDevInfo()));
    connect(ui->tBtn_hostInfo,SIGNAL(clicked(bool)),this,SIGNAL(sigBtnHostInfo()));
    connect(ui->tBtn_hostCheck,SIGNAL(clicked(bool)),this,SIGNAL(sigBtnHostCheck()));
    connect(ui->tBtn_quit,SIGNAL(clicked(bool)),this,SLOT(slotBtnQuit()));
}

void MainWindow::slotLoginStatus(int type)
{
    if(type == MySqlite::USER)
    {
        ui->tBtn_user->setText(tr("操作员"));
        ui->tBtn_record->setEnabled(true);
        ui->tBtn_hostSet->setEnabled(true);
        ui->tBtn_hostCheck->setEnabled(true);
    }
    else if(type == MySqlite::ADMIN)
    {
        ui->tBtn_user->setText(tr("管理员"));
        ui->tBtn_record->setEnabled(true);
        ui->tBtn_hostSet->setEnabled(true);
        ui->tBtn_hostCheck->setEnabled(true);
    }
    else if(type == MySqlite::QUIT)
    {
        ui->tBtn_user->setText(tr("用户登录"));
        ui->tBtn_record->setEnabled(false);
        ui->tBtn_hostSet->setEnabled(false);
        ui->tBtn_hostCheck->setEnabled(false);
    }
}

void MainWindow::HideAllTopWidget()
{

    foreach(QWidget *hubwidget,QApplication::topLevelWidgets())
    {
        if(hubwidget->objectName() == "DevInfo")//获取顶级窗口列表
        {
            if(!hubwidget->isVisible())//如果窗口不可见，显示该窗口
            {
                //QApplication::activeWindow()->hide();
                //hubwidget->show();
                //QApplication::setActiveWindow(hubwidget);
                emit sigBtnDevInfo();

            }
        }
    }
}

void MainWindow::slotBtnQuit()
{
    QApplication::quit();
}

void MainWindow::slotTimeOut()
{
    if(!g_login && g_lock == false)
    {
        g_login = true;
        HideAllTopWidget();
        emit sigLoginStatus(MySqlite::QUIT);
    }


#ifdef WatchDog
    //看门口狗
    Watchdog::kellLive();
#endif
}

void MainWindow::slotShowMain()
{
    this->show();
}



