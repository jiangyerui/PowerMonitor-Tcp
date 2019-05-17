#include "myprint.h"
#include <QDateTime>

#define POWER  4
#define BPOWER 5
#define ALARM  1
#define ERROR  2
#define CAN    3

MyPrint *MyPrint::print = NULL;

void MyPrint::initSerialPort(const QString &name)
{
//    myCom = new QSerialPort(name);
//    if(myCom->isOpen())
//    {
//        myCom->close();
//    }

//    if(myCom->open(QIODevice::ReadWrite))
//    {
//        myCom->setBaudRate(QSerialPort::Baud9600);
//        //数据位设置，我们设置为8位数据位
//        myCom->setDataBits(QSerialPort::Data8);
//        //奇偶校验设置，我们设置为无校验
//        myCom->setParity(QSerialPort::NoParity);
//        //停止位设置，我们设置为1位停止位
//        myCom->setStopBits(QSerialPort::OneStop);
//        //数据流控制设置，我们设置为无数据流控制
//        myCom->setFlowControl(QSerialPort::NoFlowControl);
//        qDebug()<<"Open print OK !";
//    }
//    else
//    {
//        qDebug()<<"Open print FAIL !";
//    }

//    //进纸命令
//    lf[0]=LF;
//    lf[1]='\0';s
}

MyPrint *MyPrint::getPrint()
{
    if(print==NULL)
    {
        print = new MyPrint();
    }
    return print;
}

void MyPrint::deletePrint()
{
    delete print;
}

void MyPrint::checkSelfPrint(QString nodeNum, QString alarmNum, QString errorNum, QString power, QString bpower)
{
//    QByteArray byte;
//    //自检完成
//    QString compelet =  tr("自检完成");
//    byte = compelet.toLocal8Bit();
//    myCom->write(byte);
//    myCom->write(lf,1);
//    //节点总数
//    QString modValue =  tr("节点总数 : ") + nodeNum;
//    byte = modValue.toLocal8Bit();
//    myCom->write(byte);
//    myCom->write(lf,1);
//    //故障数目
//    QString modAddres = tr("故障数目 : ") + errorNum;
//    byte = modAddres.toLocal8Bit();
//    myCom->write(byte);
//    myCom->write(lf,1);
//    //报警数目
//    QString modTime =   tr("报警数目 : ") + alarmNum;
//    byte = modTime.toLocal8Bit();
//    myCom->write(byte);
//    myCom->write(lf,1);
//    //主电状态
//    QString modStatus = tr("主电状态 : ") + power;
//    byte = modStatus.toLocal8Bit();
//    myCom->write(byte);
//    myCom->write(lf,1);
//    //备电状态
//    QString printType = tr("备电状态 : ") + bpower;
//    byte = printType.toLocal8Bit();
//    myCom->write(byte);
//    myCom->write(lf,1);
}

void MyPrint::printConnect(QString net, QString id, QString status,QString time, QString address)
{
//    QByteArray byte;
//    //打印地址
//    QString modAddres = tr("地址 ：") + address;
//    byte = modAddres.toLocal8Bit();
//    myCom->write(byte);
//    myCom->write(lf,1);
//    //打印时间
//    QString modTime =   tr("时间 : ") + time;
//    byte = modTime.toLocal8Bit();
//    myCom->write(byte);
//    myCom->write(lf,1);
//    //状态
//    QString modStatus = tr("状态 : ") + status;
//    byte = modStatus.toLocal8Bit();
//    myCom->write(byte);
//    myCom->write(lf,1);

//    QString modNetId=   tr("通道 : ")+ net +tr("    节点 : ")+ id;
//    byte = modNetId.toLocal8Bit();
//    myCom->write(byte);
//    myCom->write(lf,1);
}
