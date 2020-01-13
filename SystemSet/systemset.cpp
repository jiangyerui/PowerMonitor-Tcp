#include "systemset.h"
#include "ui_systemset.h"

#define KEYTIME 30*60//半小时
#define SETKEYTIME 24*60//一天的分钟数目


SystemSet::SystemSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemSet)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0,0,800,480);


    initQT();
    initVar();
    initConnect();
}

SystemSet::~SystemSet()
{
    delete ui;
}

//初始化界面jiang20190513
void SystemSet::initQT(){

    ui->pBtn_serverSet->hide();
    //本机初始化
    QString ip = m_db.getHost();
    ui->lineEdit_IP->setText(ip);

    //服务器信息初始化
    QString ip_or_name = m_db.getServerIPOrName();
    QString local_dns = m_db.getLocalDns();
    uint data_interval = m_db.getDataInterval();
    uint heart_interval = m_db.getHeartInterval();
    uint server_port = m_db.getServerPort();
    QString macAdr = m_db.getLocalMacFromDB();
//    QString macAdr = m_db.getMACAdress();
/*
    qDebug()<<"ip_or_name="+ip_or_name
              +"\n local_dns="+local_dns
              +"\n data_interval="+QString::number(data_interval)
              +"\n heart_interval="+QString::number(heart_interval)
              +"\n server_port="+QString::number(server_port)
              +"\n macAdr="+macAdr;
*/
    ui->lineEdit_server_ip_or_name->setText(ip_or_name);
    ui->lineEdit_local_dns->setText(local_dns);
    ui->lineEdit_data_interval->setText(QString::number(data_interval));
    ui->lineEdit_heart_interval->setText(QString::number(heart_interval));
    ui->lineEdit_server_port->setText(QString::number(server_port));
    ui->lineEdit_mac->setText(macAdr);
}
void SystemSet::initVar()
{
    //systemTime
    QRegExp regExpYear("^20[1-3][0-9]$");
    ui->lineEdit_year->setValidator(new QRegExpValidator(regExpYear, this));
    QRegExp regExpYeMonth("^[1-9]|(1[0-2])$");
    ui->lineEdit_month->setValidator(new QRegExpValidator(regExpYeMonth, this));
    QRegExp regExpDay("^([1-9])|([1-2][0-9])|(3[0-1])$");
    ui->lineEdit_day->setValidator(new QRegExpValidator(regExpDay, this));
    QRegExp regExpYHour("^([0-9])|(1[0-9])|(2[0-3])$");
    ui->lineEdit_hour->setValidator(new QRegExpValidator(regExpYHour, this));
    QRegExp regExpminute("^([0-9])|([1-5][0-9])$");
    ui->lineEdit_minute->setValidator(new QRegExpValidator(regExpminute, this));
    QRegExp regExpSecond("^([0-9])|([1-5][0-9])$");
    ui->lineEdit_second->setValidator(new QRegExpValidator(regExpSecond, this));
    //serialNum
    QRegExp regExpDate("^((20[0-9]{2})|(0[0-9]{2}[1-9]))(0[1-9]|1[0-2])$");
    ui->lineEdit_date->setValidator(new QRegExpValidator(regExpDate, this));
    QRegExp regExpTimes("^[0-9][0-9][1-9]$");
    ui->lineEdit_times->setValidator(new QRegExpValidator(regExpTimes, this));
    QRegExp regExpSmallType("^[1-2][1-8]$");
    ui->lineEdit_smallType->setValidator(new QRegExpValidator(regExpSmallType, this));
    //code
    QRegExp regExpNum("^[1-9][0-9]{1,5}$");//不是0开头的6位数字
    ui->lineEdit_oldPasswd->setValidator(new QRegExpValidator(regExpNum, this));
    ui->lineEdit_newPasswd->setValidator(new QRegExpValidator(regExpNum, this));
    //modSet
    QRegExp regExpMod("^([1-9][0-9])|(1[0-2][0-8])$");
    ui->lineEdit_mod->setValidator(new QRegExpValidator(regExpMod, this));
    QRegExp regExpPass("^[1-2]$");
    ui->lineEdit_pass->setValidator(new QRegExpValidator(regExpPass, this));
    QRegExp regExpRatio("^[1-9][0-9][0-9]|(1000)$");
    ui->lineEdit_ratio->setValidator(new QRegExpValidator(regExpRatio, this));

    //获取打印机配置
    setPrintType(m_db.getPrintStyle());
    //获取序列号
    m_serialNum = m_db.getSerialNumber();
    setSerialNum(m_serialNum);

    m_keyTimer = new QTimer;
    connect(m_keyTimer,SIGNAL(timeout()),this,SLOT(slotKeyTimeOut()));

    //启动
    m_keyFlag = m_db.getKeyFlag();
    if(m_keyFlag == true)
    {
        if(m_keyTimeOver == false)
        {
            m_keyTimer->start(KEYTIME*1000);
        }
    }

}

void SystemSet::initConnect()
{
    connect(ui->tBtn_quit,SIGNAL(clicked(bool)),this,SLOT(slotBtnClose()));
    connect(ui->pBtn_print,SIGNAL(clicked(bool)),this,SLOT(slotBtnPrintType()));
    connect(ui->pBtn_setType,SIGNAL(clicked(bool)),this,SLOT(slotBtnSerialNum()));
    connect(ui->pBtn_systemTime,SIGNAL(clicked(bool)),this,SLOT(slotBtnSysTime()));
    connect(ui->pBtn_systemSet,SIGNAL(clicked(bool)),this,SLOT(slotBtnSystemSet()));
    connect(ui->pBtn_updatePasswd,SIGNAL(clicked(bool)),this,SLOT(slotBtnUpdatePasswd()));
    connect(ui->pBtn_recoverPasswd,SIGNAL(clicked(bool)),this,SLOT(slotBtnRecoverPasswd()));
    connect(ui->pBtn_modRead,SIGNAL(clicked(bool)),this,SLOT(slotBtnModRead()));
    connect(ui->pBtn_modWrite,SIGNAL(clicked(bool)),this,SLOT(slotBtnModWrite()));

    connect(ui->pBtn_keyStart,SIGNAL(clicked(bool)),this,SLOT(slotBtnKeyStart()));
    connect(ui->pBtn_keyStop,SIGNAL(clicked(bool)),this,SLOT(slotBtnKeyStop()));

    connect(ui->pBtn_setIP,SIGNAL(clicked(bool)),this,SLOT(slotBtnSetHost()));


}


void SystemSet::setSerialNum(QString serialNum)
{
    if(serialNum.isEmpty())
        return;
    ui->lineEdit_type->setText(serialNum.left(2));
    ui->lineEdit_date->setText(serialNum.mid(4,6));
    ui->lineEdit_times->setText(serialNum.right(3));
    ui->lineEdit_smallType->setText(serialNum.mid(2,2));
}

void SystemSet::slotBtnClose()
{
    this->hide();
}

void SystemSet::slotBtnSysTime()
{
    if(ui->lineEdit_year->text().isEmpty()  || ui->lineEdit_month->text().isEmpty() ||
            ui->lineEdit_day->text().isEmpty()   || ui->lineEdit_hour->text().isEmpty()  ||
            ui->lineEdit_minute->text().isEmpty())
    {
        QMessageBox::information(NULL,tr("操作提示"), tr("日期/时间设置不能为空！"),tr("确定"));
        return;
    }
    int year,month,date,hour,minute,second;

    year  = ui->lineEdit_year->text().toInt();
    month = ui->lineEdit_month->text().toInt();
    date  = ui->lineEdit_day->text().toInt();
    switch (month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        break;
    case 2:
        //是闰年
        if(((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
        {
            if(date > 29)
            {
                QMessageBox::information(NULL,tr("操作提示"), tr("日期设置错误！"),tr("确定"));
                return;
            }
        }
        else
        {
            if(date > 28)
            {
                QMessageBox::information(NULL,tr("操作提示"), tr("日期设置错误！"),tr("确定"));
                return;
            }
        }
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        if(date > 30)
        {
            QMessageBox::information(NULL,tr("操作提示"), tr("日期设置错误！"),tr("确定"));
            return;
        }
        break;
    default:
        break;
    }

    hour  = ui->lineEdit_hour->text().toInt();
    minute= ui->lineEdit_minute->text().toInt();

    if(ui->lineEdit_second->text().isEmpty() == true)
    {
        second = 0;
    }
    else
    {
        second = ui->lineEdit_second->text().toInt();
    }

    time_t time;
    struct tm p;
    p.tm_year = year-1900;
    p.tm_mon  = month-1;
    p.tm_mday = date;
    p.tm_hour = hour;
    p.tm_min  = minute;
    p.tm_sec  = second;
    time = mktime(&p);
    if(time < 0)
    {
        QMessageBox::information(NULL,tr("操作提示"), tr("时间修改失败！"),tr("确定"));
        return;
    }
    if(stime(&time) < 0)
    {
        QMessageBox::information(NULL,tr("操作提示"), tr("时间修改失败！"),tr("确定"));
    }
    else
    {
        ::system("hwclock -w");
        QMessageBox::information(NULL,tr("操作提示"), tr("时间修改成功！"),tr("确定"));
    }
}

void SystemSet::slotBtnSystemSet()
{
    int ret = ::system("wr chmod -x /etc/init.d/S85qt && wr chmod +x /etc/init.d/S85Cal && reboot");
    //qDebug()<<"ret = "<< ret;
}

void SystemSet::slotBtnSerialNum()
{

    QString type  = ui->lineEdit_type->text();
    QString date  = ui->lineEdit_date->text();
    QString times = ui->lineEdit_times->text();
    QString smallType = ui->lineEdit_smallType->text();

    if( smallType.length() < 2 || date.length() < 6 || times.length() < 3)
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("主机序列号设置失败！"),tr("关闭"));
        return;
    }

    if(type.isEmpty() || date.isEmpty() || smallType.isEmpty())
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("主机序列号设置失败！"),tr("关闭"));
        return;
    }
    else
    {
        QString serialNum = type+smallType+date+times;
        qDebug()<<"serialNum : "<<serialNum;
        bool flag = m_db.insertSerialNumber(serialNum);
        if(flag)
        {
            QMessageBox::information(NULL,tr("操作提示"),tr("主机序列号设置成功！\n重启生效!"),tr("关闭"));
        }
        else
        {
            QMessageBox::information(NULL,tr("操作提示"),tr("主机序列号设置失败！"),tr("关闭"));
        }
    }
}

void SystemSet::setPrintType(bool type)
{
    if(type)
    {
        //自动打印
        ui->comboBox_print->setCurrentIndex(1);
    }
    else
    {
        //手动打印
        ui->comboBox_print->setCurrentIndex(0);
    }
}

void SystemSet::slotBtnPrintType()
{
    int index = ui->comboBox_print->currentIndex();
    if(index)
    {
        //自动打印
        m_db.setPrintStyle(true);
        QMessageBox::information(NULL,tr("操作提示"), tr("已设置为自动打印模式！"),tr("关闭"));
    }
    else
    {
        //手动打印
        m_db.setPrintStyle(false);
        QMessageBox::information(NULL,tr("操作提示"), tr("已设置为手动打印模式！"),tr("关闭"));
    }

}

void SystemSet::slotBtnUpdatePasswd()
{
    /*要根据用户登录的级别判断,执行那种权限的修改密码!
    *超级用户可以更高所有设置
    *管理员只可以更改操作员和自己的密码
    *操作员只能改自己的密码
    */
    int userType =  ui->comboBox_userType->currentIndex()+1;

    if(userType > m_userType)
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("权限不足！！！"),tr("关闭"));
        return;
    }

    QString oldPasswd = ui->lineEdit_oldPasswd->text();
    QString newPasswd = ui->lineEdit_newPasswd->text();

    if(newPasswd.isEmpty())
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("新密码不能为空！！！"),tr("关闭"));
        return;
    }

    QString code = m_db.getUserPasswd(userType);

    int result = oldPasswd.compare(code);

    if(result == 0)
    {
        if(m_db.updateUserPasswd(userType,newPasswd))
        {
            ui->lineEdit_oldPasswd->clear();ui->lineEdit_newPasswd->clear();
            QMessageBox::information(NULL,tr("操作提示"),tr("密码设置生效！！！"),tr("关闭"));
        }
    }
    else
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("原始密码错误！！！"),tr("关闭"));
    }

}

void SystemSet::slotBtnRecoverPasswd()
{
    if(m_userType < 2)
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("当前操作权限受限"),tr("关闭"));
        return;
    }
    else
    {
        m_db.recoverPasswd();
        QMessageBox::information(NULL,tr("操作提示"),tr("已经恢复出厂密码"),tr("关闭"));
    }
}

void SystemSet::slotBtnModWrite()
{
    uint id   = ui->lineEdit_mod->text().toUInt();
    uint pass = ui->lineEdit_pass->text().toUInt();
    uint type = g_mod[pass][id].type;
    if(type == MOD_VN3 || type == MOD_V3)
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("该节点不具备此项功能"),tr("关闭"));
        return;
    }
    else
    {
        uint ratio = ui->lineEdit_ratio->text().toUInt();
        g_mod[pass][id].IR = ratio;
        if(m_db.setModRatio(pass,id,ratio))
        {
            QMessageBox::information(NULL,tr("操作提示"),tr("电流变比修改成功"),tr("关闭"));
        }
        else
        {
            QMessageBox::information(NULL,tr("操作提示"),tr("电流变比修改失败"),tr("关闭"));
        }
    }
}

void SystemSet::slotKeyTimeOut()
{
    //获取当前时间
    int pastTime = m_db.getPastTime();
    if(pastTime < m_keyTotalNum)
    {
        //保存到数据库当前的时间[1]
        pastTime += 30;
        m_db.setPastTime(pastTime);
    }
    else if(pastTime == m_keyTotalNum)
    {
        m_keyTimeOver = true;
        //保存到数据库[1]
        m_db.setKeyTimeOut(1);
        //发送信号显示上锁界面[2]
        emit sigShowLock();
    }
    //qDebug()<<"****** slotKeyTimeOut ******";
}

void SystemSet::slotBtnKeyStop()
{
    if(m_db.stopSecretKeyTime())
    {
        ui->pBtn_keyStop->setEnabled(false);
        ui->pBtn_keyStart->setEnabled(true);
        ui->pBtn_keyStart->setText(tr("启 动"));
        ui->pBtn_keyStop->setText(tr("已停止"));
        ui->lineEdit_keyNum->setEnabled(true);
        QMessageBox::information(NULL,tr("操作提示"),tr("定时上锁功能已经关闭！"),tr("关闭"));
    }
    else
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("定时上锁功能关闭失败，请再次设置！"),tr("关闭"));
    }
}

void SystemSet::slotBtnKeyStart()
{

    int totalTime = ui->lineEdit_keyNum->text().toInt() * SETKEYTIME;
    if(m_db.insertSecretKeyTime(totalTime,1))
    {
        ui->pBtn_keyStop->setEnabled(true);
        ui->pBtn_keyStart->setEnabled(false);
        ui->pBtn_keyStart->setText(tr("已启动"));
        ui->pBtn_keyStop->setText(tr("停 止"));
        ui->lineEdit_keyNum->setEnabled(false);
        m_keyTimer->start(KEYTIME*1000);
        m_keyTimeOver = false;
        QMessageBox::information(NULL,tr("操作提示"),tr("定时上锁功能已经启动！"),tr("关闭"));
    }
    else
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("定时上锁功能启动失败，请再次设置！"),tr("关闭"));
    }
}

void SystemSet::slotBtnSetHost()
{
    /*
    QString host = ui->lineEdit_IP->text();
    QString macAddr = m_db.getMACAdress();
    if(macAddr.isEmpty()){
        macAddr = "00:00:01:00:00:01";
    }
    //qDebug()<<"slotBtnSetHost()macAddr="+macAddr;
    bool b = m_db.setLocalMacToDB(macAddr);
    if(b){
        qDebug()<<"set mac success !!";
    }else{
        qDebug()<<"set mac fail!!";
    }
    if(m_db.insertHost(host))
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("ip地址设置成功,即可重启！"),tr("关闭"));
        ::system("wr reboot");
    }
    else
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("ip地址设置失败！"),tr("关闭"));
    }
    */


    bool b1 = false;
    bool b2 = false;
    bool b3 = false;
    bool b4 = false;
    bool b5 = false;
    bool b6 = false;

    QString ip_or_name = ui->lineEdit_server_ip_or_name->text();
    QString local_dns = ui->lineEdit_local_dns->text();//网关
    QString data_interval = ui->lineEdit_data_interval->text();
    QString heart_interval = ui->lineEdit_heart_interval->text();
    QString server_port = ui->lineEdit_server_port->text();

    b1 = m_db.setServerIPOrName(ip_or_name);
    b2 = m_db.setLocalDns(local_dns);
    b3 = m_db.setDataInterval(data_interval.toInt());
    b4 = m_db.setHeartInterval(heart_interval.toInt());
    b5 = m_db.setServerPort(server_port.toInt());




    //
    QString host = ui->lineEdit_IP->text();
    b6 = m_db.insertHost(host);

    QString macAddr = m_db.getMACAdress();
    if(macAddr.isEmpty()){
        macAddr = "00:00:01:00:00:01";
    }
    //qDebug()<<"slotBtnSetHost()macAddr="+macAddr;
    bool b = m_db.setLocalMacToDB(macAddr);
    if(b){
        qDebug()<<"set mac success !!";
    }else{
        qDebug()<<"set mac fail!!";
    }
    //


    if(b1&&b2&&b3&&b4&&b5&&b6){
        QMessageBox::information(NULL,tr("操作提示"),tr("服务器设置成功,即可重启！"),tr("关闭"));
        ::system("wr reboot");
    }
    else{
        QMessageBox::information(NULL,tr("操作提示"),tr("服务器设置失败！"),tr("关闭"));
    }
}

void SystemSet::slotBtnModRead()
{
    uint id   = ui->lineEdit_mod->text().toInt();
    uint pass = ui->lineEdit_pass->text().toInt();
    uint type = g_mod[pass][id].type;

    if(type != MOD_VN3)
    {
        ui->lineEdit_ratio->setText(QString::number(g_mod[pass][id].IR));
    }
    else
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("该节点不具备此项功能"),tr("关闭"));
        return;
    }

}

void SystemSet::slotSystemShow()
{
    this->show();
    setSerialNum(m_serialNum);
    ui->lineEdit_newPasswd->clear();
    ui->lineEdit_oldPasswd->clear();
    //获取总共的天数
    uint keyNum = m_db.getTotleTime()/24/60;
    ui->lineEdit_keyNum->setText(QString::number(keyNum));
    //判断密钥是否启动
    m_keyFlag = m_db.getKeyFlag();
    if(m_keyFlag == true)
    {
        ui->pBtn_keyStop->setEnabled(true);
        ui->pBtn_keyStart->setEnabled(false);
        ui->pBtn_keyStart->setText(tr("已启动"));
        ui->pBtn_keyStop->setText(tr("停 止"));
        ui->lineEdit_keyNum->setEnabled(false);
    }
    else
    {
        ui->pBtn_keyStop->setEnabled(false);
        ui->pBtn_keyStart->setEnabled(true);
        ui->pBtn_keyStart->setText(tr("启 动"));
        ui->pBtn_keyStop->setText(tr("已停止"));
        ui->lineEdit_keyNum->setEnabled(true);
    }

}

void SystemSet::slotLoginStatus(int type)
{
    m_userType = type;
    if(m_userType != MySqlite::ADMIN)
    {
        ui->pBtn_setType->setEnabled(false);
        ui->lineEdit_date->setEnabled(false);
        ui->lineEdit_times->setEnabled(false);
        ui->lineEdit_smallType->setEnabled(false);
        ui->pBtn_modWrite->setEnabled(false);
        ui->pBtn_modRead->setEnabled(false);
    }
    else
    {
        ui->pBtn_setType->setEnabled(true);
        ui->lineEdit_date->setEnabled(true);
        ui->lineEdit_times->setEnabled(true);
        ui->lineEdit_smallType->setEnabled(true);
        ui->pBtn_modRead->setEnabled(true);
        ui->pBtn_modWrite->setEnabled(true);
    }
}


//服务器设置jiang20190513//本函数废弃20190926
void SystemSet::on_pBtn_serverSet_clicked()
{
    bool b1 = false;
    bool b2 = false;
    bool b3 = false;
    bool b4 = false;
    bool b5 = false;

    QString ip_or_name = ui->lineEdit_server_ip_or_name->text();
    QString local_dns = ui->lineEdit_local_dns->text();
    QString data_interval = ui->lineEdit_data_interval->text();
    QString heart_interval = ui->lineEdit_heart_interval->text();
    QString server_port = ui->lineEdit_server_port->text();

    b1 = m_db.setServerIPOrName(ip_or_name);
    b2 = m_db.setLocalDns(local_dns);
    b3 = m_db.setDataInterval(data_interval.toInt());
    b4 = m_db.setHeartInterval(heart_interval.toInt());
    b5 = m_db.setServerPort(server_port.toInt());



    //
    QString host = ui->lineEdit_IP->text();
    QString macAddr = m_db.getMACAdress();
    if(macAddr.isEmpty()){
        macAddr = "00:00:01:00:00:01";
    }
    //qDebug()<<"slotBtnSetHost()macAddr="+macAddr;
    bool b = m_db.setLocalMacToDB(macAddr);
    if(b){
        qDebug()<<"set mac success !!";
    }else{
        qDebug()<<"set mac fail!!";
    }
    //


    if(b1&&b2&&b3&&b4&&b5){
        QMessageBox::information(NULL,tr("操作提示"),tr("服务器设置成功,即可重启！"),tr("关闭"));
        ::system("wr reboot");
    }
    else{
        QMessageBox::information(NULL,tr("操作提示"),tr("服务器设置失败！"),tr("关闭"));
    }

}
