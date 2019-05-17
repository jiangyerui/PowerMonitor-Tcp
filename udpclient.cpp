#include "udpclient.h"
#include <QDebug>
#include <QThread>
#include <QTime>
#include <QCoreApplication>
#include <QHostInfo>
//#include <QList>
#include <QProcess>


UdpClient::UdpClient(QObject *parent) : QObject(parent)
{
    //qDebug()<<"UdpClient()";

    m_udpClient = new QUdpSocket;
    //m_udpServer = new QUdpSocket(this);
    //QString host = udp_db.getHost();
    m_udpClient->bind(6001);
    //m_udpServer->bind(6062,QUdpSocket::ShareAddress);


    //初始化服务器IP或者域名
    serverIPOrName = udp_db.getServerIPOrName();
    serverHost = (QHostAddress)serverIPOrName;
    //初始化服务器端口
    PORT = udp_db.getServerPort();
    //初始化数据包的发送时间间隔
    uint data_interval = udp_db.getDataInterval();
    //qDebug()<<"data_interval----------------"+QString::number(data_interval);
    //初始化心跳包的发送时间间隔
    uint heart_interval = udp_db.getHeartInterval();
    //qDebug()<<"heart_interval----------------"+QString::number(heart_interval);

    //发数据包
    timer = new QTimer(this);
    timer->setInterval(data_interval*1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(sendData()));

    //发送心跳包
    timerHeart = new QTimer(this);
    timerHeart->setInterval(heart_interval*1000);
    connect(timerHeart,SIGNAL(timeout()),this,SLOT(sendHeartData()));

    //接收服务器下发的指令
    connect(m_udpClient,SIGNAL(readyRead()),this,SLOT(recvServerData()));
    //connect(m_udpClient,&QUdpSocket::readyRead,this,&UdpClient::recvServerData);


    //联服务器
    timer1 = new QTimer(this);
    connect(timer1,SIGNAL(timeout()),this,SLOT(getServerHost()));
    timer1->start(10000);

    //联服务器成功后发数据
    connect(this,SIGNAL(getServerSuccess()),this,SLOT(startSendTimer()));
}

//初始化要发送的数据包
void UdpClient::init_m_byteArray(){
    //qDebug()<<"init_m_byteArray()";
    m_byteArray[0] = 0xAA; //固定帧头
    m_byteArray[7] = 0x02;  //数据来源：网关
    m_byteArray[10] = 0x00; //生产商编号：0x000001
    m_byteArray[11] = 0x00;
    m_byteArray[12] = 0x01;
    m_byteArray[13] = 0x00; //协议编号：0x0005 电压电流网关－>服务器
    m_byteArray[14] = 0x05;

    //QString macStr = udp_db.getMACAdress();
    QString macStr = udp_db.getLocalMacFromDB();
    QStringList list = macStr.split(":");
    if(!list.isEmpty()){
        for(int i=0;i<6;i++){
            QString str = list.at(i);
            QByteArray senddata = QString2Hex(str);
            //qDebug()<<"senddata[0]======"+QString::number(senddata[0]);
            if(!senddata.isEmpty()){
                m_byteArray[15+i] = senddata[0]; //Mac地址
            }

        }
    }else{
        qDebug()<<"get Mac address fail!!";
    }
    m_byteArray[21] = 0x01; //数据长度?
    m_byteArray[22] = 0x5D;
    m_byteArray[23] = 0x05; //模块类型：消防电源主机
    m_byteArray[25] = 0x10; //探测器数量
}

//初始化心跳包
void UdpClient::init_m_heartArray(){
    //qDebug()<<"init_m_heartArray()";
    m_heartArray[0] = 0xAA; //固定帧头
    m_heartArray[7] = 0x02;  //数据来源：网关
    m_heartArray[8] = 0x01; //数据类型：0X01心跳数据
    m_heartArray[9] = 0x00; //事件代码 ：0X00无事件
    m_heartArray[10] = 0x00; //生产商编号：0x000001
    m_heartArray[11] = 0x00;
    m_heartArray[12] = 0x01;
    m_heartArray[13] = 0x00; //协议编号：0x0005 电压电流网关－>服务器
    m_heartArray[14] = 0x05;

//    QString macStr = udp_db.getMACAdress();
    QString macStr = udp_db.getLocalMacFromDB();
    QStringList list = macStr.split(":");
    if(!list.isEmpty()){
        for(int i=0;i<6;i++){
            QString str = list.at(i);
            QByteArray senddata = QString2Hex(str);
            if(!senddata.isEmpty()){
                m_heartArray[15+i] = senddata[0]; //Mac地址
            }
        }
    }else{
        qDebug()<<"get Mac address fail!!";
    }
    m_heartArray[21] = 0x00; //数据长度
    m_heartArray[22] = 0x00;

}


//初始化回应服务器的数据包
void UdpClient::init_m_returnArray(){
    m_returnArray[0] = 0xAA; //固定帧头
    m_returnArray[7] = 0x02;  //数据来源：网关
    m_returnArray[8] = 0x82; //数据类型：事件数据回复
    m_returnArray[9] = 0x0A; //事件代码 ：数据处理成功
    m_returnArray[10] = 0x00; //生产商编号：0x000001
    m_returnArray[11] = 0x00;
    m_returnArray[12] = 0x01;
    //m_returnArray[13] = 0x00; //协议编号：0x0001 网关－>服务器
    //m_returnArray[14] = 0x01;

//    QString macStr = udp_db.getMACAdress();
    QString macStr = udp_db.getLocalMacFromDB();
    QStringList list = macStr.split(":");
    if(!list.isEmpty()){
        for(int i=0;i<6;i++){
            QString str = list.at(i);
            QByteArray senddata = QString2Hex(str);
            if(!senddata.isEmpty()){
                m_returnArray[15+i] = senddata[0]; //Mac地址
            }
        }
    }else{
        qDebug()<<"get Mac address fail!!";
    }

    m_returnArray[21] = 0x00; //数据长度
    m_returnArray[22] = 0x00;

}



//十六进制的字串转为QByteArray
char UdpClient::ConvertHexChar(char ch)
{
    //qDebug()<<"ConvertHexChar()";
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}
QByteArray UdpClient::QString2Hex(QString str)
{
    //qDebug()<<"QString2Hex()";
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;

}


//延时
void UdpClient::mySleep(int sec)
{
    QTime dieTime = QTime::currentTime().addMSecs(sec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


//crc16相关jiang20190514
uint UdpClient::GetCrc_16(unsigned char *pData, int nLength, unsigned short init, const unsigned short *ptable) {
   //qDebug()<<"GetCrc_16()";
    unsigned short cRc_16 = init;
    unsigned char temp;

    while(nLength-- > 0) {
        temp = cRc_16 & 0xFF;
        cRc_16 = (cRc_16 >> 8) ^ ptable[(temp ^ *pData++) & 0xFF];
    }

    return cRc_16;
}
//Modbus CRC16校验
uint UdpClient::CRC_GetModbus16(unsigned char *pdata, int len) {
    //qDebug()<<"CRC_GetModbus16()";
    //MODBUS CRC-16表 8005 逆序
    const unsigned short g_McRctable_16[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };

    return GetCrc_16(pdata, len, 0xFFFF, g_McRctable_16);
}

//CRC16校验
uint UdpClient::GetCRCCode(QByteArray data) {
    //qDebug()<<"GetCRCCode()";
    uint result = CRC_GetModbus16((unsigned char *)data.data(), data.length());
    return result;
}

//每次发送16台探测器的数据
void UdpClient::sendOnceData(uint pass,uint startCanId)
{
    //qDebug()<<"sendOnceData()";
    //获取时间戳
    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch(); //毫秒级
    //qDebug()<<"timestamp------------ms------"+QString::number(timestamp);

    //QDateTime time = QDateTime::currentDateTime(); //获取当前时间
    //uint cut_timestamp = time.toTime_t(); //将当前时间转为时间戳
    //qDebug()<<"cut_timestamp-----------s-------"+QString::number(timestamp);
    //QDateTime time_1= QDateTime::fromTime_t(cut_timestamp); //将时间戳转为时间


    m_byteArray[1] = timestamp>>40;
    m_byteArray[2] = timestamp>>32;
    m_byteArray[3] = timestamp>>24;
    m_byteArray[4] = timestamp>>16;
    m_byteArray[5] = timestamp>>8;
    m_byteArray[6] = timestamp;
    for(int i=0;i<16;i++){
        //探测器状态
        //0x00探测器通信中
        //0x01探测器正常
        //0x02探测器报警：供电中断
        //0x03探测器故障：过欠流、过欠压、错相
        //0x04探测器通信故障
        //0x05探测器不在线
        uint modeSts = 0x05;
        if(g_mod[pass][startCanId+i].normalFlag == TRUE)//正常
        {
            modeSts = 0x01;
        }
        if(g_mod[pass][startCanId+i].overCurrentFlag == TRUE)//过流
        {
            modeSts = 0x03;
        }
        if(g_mod[pass][startCanId+i].phaseLossFlag == TRUE)//错相
        {
            modeSts = 0x03;
        }
        if(g_mod[pass][startCanId+i].overVoltageFlag == TRUE)//过压
        {
            modeSts = 0x03;
        }
        if(g_mod[pass][startCanId+i].underVoltageFlag == TRUE)//欠压
        {
            modeSts = 0x03;
        }
        if(g_mod[pass][startCanId+i].dropFlag == TRUE)//掉线
        {
            modeSts = 0x04;
        }
        if(g_mod[pass][startCanId+i].interruptionFlag == TRUE)//供电中断
        {
            modeSts = 0x02;
        }
        uint av_1 = g_mod[pass][startCanId+i].AV_1;
        uint bv_1 = g_mod[pass][startCanId+i].BV_2;
        uint cv_1 = g_mod[pass][startCanId+i].CV_3;
        uint av_2 = g_mod[pass][startCanId+i].AV_4;
        uint bv_2 = g_mod[pass][startCanId+i].BV_5;
        uint cv_2 = g_mod[pass][startCanId+i].CV_6;
        uint ai_1 = g_mod[pass][startCanId+i].AI_1;
        uint bi_1 = g_mod[pass][startCanId+i].BI_2;
        uint ci_1 = g_mod[pass][startCanId+i].CI_3;

        m_byteArray[26+i*20] = startCanId+i;//探测器索引
        m_byteArray[27+i*20] = modeSts;//探测器状态
        m_byteArray[28+i*20] = av_1>>8;//av_1高8位
        m_byteArray[29+i*20] = av_1;//av_低8位
        m_byteArray[30+i*20] = bv_1>>8;
        m_byteArray[31+i*20] = bv_1;
        m_byteArray[32+i*20] = cv_1>>8;
        m_byteArray[33+i*20] = cv_1;
        m_byteArray[34+i*20] = av_2>>8;
        m_byteArray[35+i*20] = av_2;
        m_byteArray[36+i*20] = bv_2>>8;
        m_byteArray[37+i*20] = bv_2;
        m_byteArray[38+i*20] = cv_2>>8;
        m_byteArray[39+i*20] = cv_2;
        m_byteArray[40+i*20] = ai_1>>8;
        m_byteArray[41+i*20] = ai_1;
        m_byteArray[42+i*20] = bi_1>>8;
        m_byteArray[43+i*20] = bi_1;
        m_byteArray[44+i*20] = ci_1>>8;
        m_byteArray[45+i*20] = ci_1;
    }

    uint m_crc16 = GetCRCCode(m_byteArray);
    m_byteArray[346] = m_crc16>>8;//CRC16校验,高8位
    m_byteArray[347] = m_crc16;//CRC16校验,低8位
    m_byteArray[348] = 0x55;//固定帧尾

    //发送到服务器
    int ret = m_udpClient->writeDatagram(m_byteArray,serverHost,PORT);
    if(ret<0){
        qDebug()<<"+++++++++++++++sendData fail++++++++++++++++";
    }

}

//发送数据包
void UdpClient::sendData(){
    uint pass = 1;
    uint startCanId = sendDataCount*16+1;
    sendOnceData(pass,startCanId);
    sendDataCount++;
    if(sendDataCount>7){
        sendDataCount=0;
    }
}

//发送心跳包
void UdpClient::sendHeartData(){
    //qDebug()<<"sendHeartData()";
    //获取时间戳
    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch(); //毫秒级

    m_heartArray[1] = timestamp>>40;
    m_heartArray[2] = timestamp>>32;
    m_heartArray[3] = timestamp>>24;
    m_heartArray[4] = timestamp>>16;
    m_heartArray[5] = timestamp>>8;
    m_heartArray[6] = timestamp;

    uint m_crc16 = GetCRCCode(m_heartArray);
    m_heartArray[23] = m_crc16>>8;//CRC16校验,高8位
    m_heartArray[24] = m_crc16;//CRC16校验,低8位
    m_heartArray[25] = 0x55; //帧尾：0X55

    //发送到服务器
    int ret = m_udpClient->writeDatagram(m_heartArray,serverHost,PORT);
    if(ret<0){
        qDebug()<<"+++++++++++++++sendHeartData fail++++++++++++++++";
    }
}


//接收服务器的下发指令
void UdpClient::recvServerData(){
    //qDebug()<<"------------recvServerData----------------";
    QHostAddress addr;
    quint16 port;
    m_recvArray.resize(m_udpClient->pendingDatagramSize());
    m_udpClient->readDatagram(m_recvArray.data(),m_recvArray.size(),&addr,&port);
    //解析服务器下发的指令
    if(m_recvArray[0]==0xAA&&m_recvArray[7]==0x05&&m_recvArray[8]==0x02&&m_recvArray[9]==0x06){
        if(m_recvArray[15]==m_byteArray[15]
                &&m_recvArray[16]==m_byteArray[16]
                &&m_recvArray[17]==m_byteArray[17]
                &&m_recvArray[18]==m_byteArray[18]
                &&m_recvArray[19]==m_byteArray[19]
                &&m_recvArray[20]==m_byteArray[20]
                ){
            if(m_recvArray[23]==0x01){
                //复位s
                qDebug()<<"-----------------------reset------------------";
                //数据库删除
                udp_db.delData("delete from TEMP_ERROR;");//删除临时记录
                udp_db.delData("delete from TEMP_POWER;");//删除临时记录
                for(int net = 1;net <=2;net++)
                {
                    for(int id = 1;id<= 128;id++)
                    {
                        g_mod[net][id].dropFlag           = false;
                        g_mod[net][id].normalFlag         = true;
                        g_mod[net][id].phaseLossFlag      = false;
                        g_mod[net][id].overCurrentFlag    = false;
                        g_mod[net][id].overVoltageFlag    = false;
                        g_mod[net][id].underVoltageFlag   = false;
                        g_mod[net][id].interruptionFlag   = false;
                        g_mod[net][id].insertDrop         = false;
                        g_mod[net][id].insertphaseLoss    = false;
                        g_mod[net][id].insertOverCurrent  = false;
                        g_mod[net][id].insertOverVoltage  = false;
                        g_mod[net][id].insertUnderVoltage = false;
                        g_mod[net][id].insertInterruption = false;
                    }
                    //消息ID，网关原值返回
                    m_returnArray[1] = m_recvArray[1];
                    m_returnArray[2] = m_recvArray[2];
                    m_returnArray[3] = m_recvArray[3];
                    m_returnArray[4] = m_recvArray[4];
                    m_returnArray[5] = m_recvArray[5];
                    m_returnArray[6] = m_recvArray[6];
                    //协议编号，网关原值返回
                    m_returnArray[13] = m_recvArray[13];
                    m_returnArray[14] = m_recvArray[14];

                    //CRC16
                    uint m_crc16 = GetCRCCode(m_returnArray);
                    m_returnArray[23] = m_crc16>>8;//CRC16校验,高8位
                    m_returnArray[24] = m_crc16;//CRC16校验,低8位
                    m_returnArray[25] = 0x55; //帧尾：0X55

                    //发送到服务器
                    m_udpClient->writeDatagram(m_returnArray,serverHost,PORT);
                }

            }else if(m_recvArray[23]==0x02){
                //消音
                qDebug()<<"-----------------------cancel------------------";
                emit sigSoundType(S_NORMAL);
                //消息ID，网关原值返回
                m_returnArray[1] = m_recvArray[1];
                m_returnArray[2] = m_recvArray[2];
                m_returnArray[3] = m_recvArray[3];
                m_returnArray[4] = m_recvArray[4];
                m_returnArray[5] = m_recvArray[5];
                m_returnArray[6] = m_recvArray[6];
                //协议编号，网关原值返回
                m_returnArray[13] = m_recvArray[13];
                m_returnArray[14] = m_recvArray[14];

                //CRC16
                uint m_crc16 = GetCRCCode(m_returnArray);
                m_returnArray[23] = m_crc16>>8;//CRC16校验,高8位
                m_returnArray[24] = m_crc16;//CRC16校验,低8位
                m_returnArray[25] = 0x55; //帧尾：0X55

                //发送到服务器
                m_udpClient->writeDatagram(m_returnArray,serverHost,PORT);
            }else{
            }
        }
    }
//    for(int i=0;i<m_recvArray.size();i++){
//        m_recvArray[i];
//        qDebug()<<"==========---datagram--+"+QString::number(i)+"+---"+QString::number(m_recvArray[i]);
//    }

}



//连接服务器
void UdpClient::getServerHost()
{
    //qDebug()<<"getServerHost()";
    QString str = udp_db.getServerIPOrName();
    //QHostInfo info = QHostInfo::fromName("www.lclot.com");
    QHostInfo info = QHostInfo::fromName(str);//str可以是IP，也可以是域名

    QList<QHostAddress> list = info.addresses();
    if(!list.isEmpty()){
        timer1->stop();
        serverHost = list.first();
        //qDebug()<<"getServerHost ======== success!!serverHost="+str+"-------//port="+QString::number(PORT);
        emit getServerSuccess();
    }else{
        int ret = ::system("echo nameserver 219.141.136.10 >> /etc/resolv.conf");
        qDebug()<<"getServerHost() error";
    }
}

//开启发送数据包的定时器
void UdpClient::startSendTimer()
{
    qDebug()<<"server connect success,udp send start....";
    //qDebug()<<"startSendTimer()";
    timer->start();
    timerHeart->start();

    //初始化数据
    //初始化数组
    init_m_byteArray();
    init_m_heartArray();
    init_m_returnArray();

}



