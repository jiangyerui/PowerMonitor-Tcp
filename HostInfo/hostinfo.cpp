#include "hostinfo.h"
#include "ui_hostinfo.h"

HostInfo::HostInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HostInfo)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    initVar();
    connect(ui->tBtn_quit,SIGNAL(clicked(bool)),this,SLOT(slotTBtnQuit()));
}

HostInfo::~HostInfo()
{
    delete ui;
}

void HostInfo::initVar()
{
    QString serialNum =m_db.getSerialNumber();
    QString str = QString("%1-%2-%3-%4").arg(serialNum.left(2)).arg(serialNum.mid(2,2)).arg(serialNum.mid(4,6)).arg(serialNum.right(3));
    ui->lb_hostSerial->setText(str);
}

void HostInfo::slotTBtnQuit()
{
    this->hide();
    emit sigMainPageShow();
}

void HostInfo::slotHostInfoShow()
{

    this->show();
}
