#include "userlogin.h"
#include "ui_userlogin.h"
#include "QMessageBox"
#include "GlobalData/globaldata.h"
UserLogin::UserLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserLogin)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0,0,800,480);
    initVar();

    connect(ui->pBtn_ok,SIGNAL(clicked(bool)),this,SLOT(slotBtnOk()));
    connect(ui->pBtn_cancel,SIGNAL(clicked(bool)),this,SLOT(slotBtnCancel()));
}

UserLogin::~UserLogin()
{
    delete ui;
}

void UserLogin::initVar()
{
    m_keyTimeOut = false;
    m_user.clear();m_admin.clear();
    //第一位不是0,2-6位可以0-9
    QRegExp regExpDate("^([1-9][0-9]{1,5})$");
    ui->lineEdit_passwd->setValidator(new QRegExpValidator(regExpDate, this));
}

void UserLogin::slotBtnOk()
{
    if(m_keyTimeOut == true)
    {
        ui->lineEdit_key->show();
        ui->lineEdit_passwd->hide();
        QString key = ui->lineEdit_key->text();
        if(key == m_db.getSecretKey())
        {
            m_keyTimeOut = false;
            QMessageBox::information(NULL,tr("信息提示"),tr("密钥正确！"),tr("确定"));
            //设置密钥复位
            m_db.stopSecretKeyTime();
            g_lock = false;
            emit sigMainPageShow();
            this->hide();
        }
        else
        {
            QMessageBox::information(NULL,tr("信息提示"),tr("密钥错误！"),tr("确定"));
        }
    }
    else
    {
        ui->lineEdit_key->hide();
        ui->lineEdit_passwd->show();
        QString passWd = ui->lineEdit_passwd->text();
        int index = ui->comboBox->currentIndex() + 1;
        if(m_user == passWd && index == MySqlite::USER)
        {
            emit sigLoginStatus(MySqlite::USER);
            this->hide();
        }
        else if(m_admin == passWd && index == MySqlite::ADMIN)
        {
            emit sigLoginStatus(MySqlite::ADMIN);
            this->hide();
        }
        else if(index == MySqlite::QUIT)
        {
            emit sigLoginStatus(MySqlite::QUIT);
            this->hide();
        }
        else
        {
            QMessageBox::information(NULL,tr("信息提示"),tr("密码错误！"),tr("确定"));
        }
    }
}



void UserLogin::slotBtnCancel()
{
    this->hide();
}

void UserLogin::slotLoginShow()
{

    show();
    m_user  = m_db.getUserPasswd(MySqlite::USER);
    m_admin = m_db.getUserPasswd(MySqlite::ADMIN);
    qDebug()<<m_user;
    qDebug()<<m_admin;
    //获取序列号
    QString serialNum = m_db.getSerialNumber();
    QString str = QString("%1-%2-%3-%4").arg(serialNum.left(2)).arg(serialNum.mid(2,2)).arg(serialNum.mid(4,6)).arg(serialNum.right(3));
    ui->lb_hostSerial->setText(str);
    ui->lb_msg->setFocus();
    ui->lb_msg->setText(tr("欢迎您登录本系统\n请输入该设备的密码"));

    ui->lineEdit_passwd->show();
    ui->lineEdit_passwd->clear();

    ui->lineEdit_key->hide();
    ui->lineEdit_key->clear();

    ui->pBtn_cancel->setEnabled(true);

}

void UserLogin::showLock()
{
    show();
    //获取序列号
    QString serialNum = m_db.getSerialNumber();
    QString str = QString("%1-%2-%3-%4").arg(serialNum.left(2)).arg(serialNum.mid(2,2)).arg(serialNum.mid(4,6)).arg(serialNum.right(3));
    ui->lb_hostSerial->setText(str);
    ui->lb_msg->setFocus();
    ui->lb_msg->setText(tr("欢迎您登录本系统\n请输入该设备的密钥"));

    ui->lineEdit_passwd->hide();
    ui->lineEdit_passwd->clear();
    ui->lineEdit_key->show();
    ui->lineEdit_key->clear();

    ui->pBtn_cancel->setEnabled(false);
    //m_keyTimeOut = true;
    g_lock = true;
}



