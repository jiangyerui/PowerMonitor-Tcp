#include "mysqlite.h"
#include <QDebug>
#include <QCryptographicHash>

MySqlite::MySqlite(QObject *parent) : QObject(parent)
{

}

void MySqlite::initDb()
{

    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        m_db = QSqlDatabase::addDatabase("QSQLITE"); //添数据库驱
        m_db.setDatabaseName("./PowerMonitor.db");
        //m_db.setDatabaseName("./Bevone.db");
        if (m_db.open())
        {
            qDebug("**** MySqlite open success ****") ;
        }
        else
        {
            qDebug("**** MySqlite open error ****");
        }
    }

}

void MySqlite::insertNode(int net, int id, QString address)
{
    QString netStr  = QString::number(net);
    QString idStr   = QString::number(id);
    QString sql = "insert into NODE values("+netStr+","+idStr+", '"+address+"');";
    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();

}

void MySqlite::insertTemp(int net, int id, int type,uint time)
{
    QString netStr  = QString::number(net);
    QString idStr   = QString::number(id);
    QString typeStr = QString::number(type);
    QString timeStr = QString::number(time);

    QString sql;
    if(type == 0x05)
    {
        sql = "insert into TEMP_POWER values("+netStr+","+idStr+","+typeStr+","+timeStr+");";
    }
    else
    {
        sql = "insert into TEMP_ERROR values("+netStr+","+idStr+","+typeStr+","+timeStr+");";
    }

    //qDebug()<<"insertTemp = "<<sql;
    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();
}

void MySqlite::insertAlarm(int net, int id, int status, uint time, QString address)
{
    /*
     * 获取MyPrint实例
    */
    QString netStr  = QString::number(net);
    QString idStr   = QString::number(id);
    QString timeStr = QString::number(time);
    QString staStr  = QString::number(status);
    QString sql = "insert into RECORD values("+netStr+","+idStr+","+staStr+","+timeStr+","+"'"+address+"');";
    //qDebug()<<"sql : "<<sql;
    m_db.transaction();
    QSqlQuery query(m_db);
    query.exec(sql);
    m_db.commit();
    query.clear();
}


bool MySqlite::insertSerialNumber(QString serialNO)
{

    bool flag = false;
    if(NULL == serialNO)
        return flag;
    QByteArray bytePwd = serialNO.toLatin1();
    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    QString strPwdMd5 = (bytePwdMd5.toHex().left(12)).toUpper();
    setSecretKey(strPwdMd5);
    qDebug()<<"strPwdMd5 = "<<strPwdMd5;

    QString sql = "update SERIALNUM set NO = '"+serialNO+"' WHERE rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        flag = true;
    }
    else
    {
        flag = false;
    }
    query.clear();
    return flag;

}

QString MySqlite::getSerialNumber()
{
    QString serialNum;
    QString sql = "select NO from SERIALNUM where rowid = 1;";
    QSqlQuery query(m_db);
    bool flag = query.exec(sql);

    if(flag)
    {
        if(query.next())
        {
            serialNum  = query.value(0).toString();
        }
    }
    query.clear();
    return serialNum;
}

QString MySqlite::getNodeAddress(int net, int id)
{
    QString addStr;
    QString netStr = QString::number(net);
    QString idStr = QString::number(id);
    QString sql = "select address from NODE where net = "+netStr+" and id ="+idStr+";";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            addStr = query.value(0).toString();
        }
    }
    query.clear();
    return addStr;

}

uint MySqlite::getModRatio(int net, int id)
{
    uint modRatio;
    QString netStr = QString::number(net);
    QString idStr = QString::number(id);
    QString sql = "select ratio from NODE where net = "+netStr+" and id ="+idStr+";";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            modRatio = query.value(0).toUInt();
        }
    }
    query.clear();
    return modRatio;
}

bool MySqlite::setModRatio(int net, int id, uint ratio)
{
    bool flag = false;
    QString netStr = QString::number(net);
    QString idStr  = QString::number(id);
    QString ratioStr = QString::number(ratio);
    QString sql = "update NODE set ratio = "+ratioStr+" where net = "+netStr+" and id = "+idStr+" ;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        flag = true;
    }
    query.clear();
    return flag;
}

bool MySqlite::getPrintError()
{
    bool printFlag = false;
    QString sql = "select ERROR from PRINT where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            printFlag  = query.value(0).toBool();
        }
    }
    query.clear();
    return printFlag;
}

void MySqlite::setPrintError(bool model)
{
    QString sql;
    sql.clear();
    if(model == true)
    {
        sql = "update PRINT set ERROR = 1 where rowid = 1;";
    }
    else
    {
        sql = "update PRINT set ERROR = 0 where rowid = 1;";
    }
    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();
}

void MySqlite::setPrintAlarm(bool model)
{
    QString sql;
    sql.clear();
    if(model == true)
    {
        sql = "update PRINT set ALARM = 1 where rowid = 1;";
    }
    else
    {
        sql = "update PRINT set ALARM = 0 where rowid = 1;";
    }
    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();
}

bool MySqlite::getPrintAlarm()
{
    bool printFlag = false;
    QString sql = "select ALARM from PRINT where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            printFlag  = query.value(0).toBool();
        }
    }
    query.clear();
    return printFlag;
}

void MySqlite::setPrintStyle(bool model)
{
    QString sql;
    sql.clear();
    if(model == true)
    {
        sql = "update PRINT set MODE = 1 where rowid = 1;";
    }
    else
    {
        sql = "update PRINT set MODE = 0 where rowid = 1;";
    }
    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();
}

bool MySqlite::getPrintStyle()
{
    bool printFlag = false;
    QString sql = "select MODE from PRINT where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            printFlag  = query.value(0).toBool();
        }
    }
    query.clear();
    return printFlag;
}

bool MySqlite::updateUserPasswd(int userType, QString passwd)
{
    bool flag = false;
    QString sql;
    sql.clear();
    sql = "update PASSWD set CODE = "+passwd+" where USER = "+QString::number(userType)+";";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        flag = true;
    }
    query.clear();
    return flag;
}

bool MySqlite::setSecretKey(QString secretKey)
{
    bool KeyFlag = false;
    QString sql = "update SECRETKEY set KEY = '"+secretKey+"' where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        KeyFlag = true;
    }
    query.clear();
    return KeyFlag;
}

QString MySqlite::getSecretKey()
{
    QString secretKey;
    QString sql = "select KEY from SECRETKEY where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            secretKey = query.value(0).toString();
        }
    }
    query.clear();
    return secretKey;
}

bool MySqlite::getKeyFlag()
{
    bool KeyFlag = false;
    QString sql = "select KEYFLAG from SECRETKEY where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            KeyFlag = query.value(0).toBool();
        }
    }
    query.clear();
    return KeyFlag;
}

bool MySqlite::getKeyTimeOut()
{
    bool KeyTimeOut = false;
    QString sql = "select KEYTIMEOUT from SECRETKEY where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            KeyTimeOut = query.value(0).toBool();
        }
    }
    query.clear();
    return KeyTimeOut;
}

bool MySqlite::setKeyTimeOut(int timeFlag)
{

    bool KeyTimeOut = false;
    QString sql;
    sql.clear();
    sql = "update SECRETKEY set KEYTIMEOUT = "+QString::number(timeFlag)+" where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        KeyTimeOut = true;
    }
    query.clear();
    return KeyTimeOut;
}

QString MySqlite::getUserPasswd(int userType)
{
    QString passwd;
    QString sql = "select CODE from PASSWD where USER = "+QString::number(userType)+";";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            passwd = query.value(0).toString();
        }
    }
    query.clear();
    return passwd;
}

bool MySqlite::setPollTime(QString time)
{
    bool flag = false;
    QString sql;
    sql.clear();
    sql = "update SYSTEM set polltime = "+time+" where rowid = 1;";
    QSqlQuery query(m_db);
    flag = query.exec(sql);
    query.clear();
    return flag;
}

uint MySqlite::getPollTime()
{
    uint pollTime = 0;
    QString sql = "select polltime from SYSTEM where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            pollTime  = query.value(0).toULongLong();
        }
    }
    query.clear();
    return pollTime;
}

uint MySqlite::getRowCount(QString &sql, QSqlQueryModel *model)
{
    int r=0;
    model->setQuery(sql,m_db);//链接数据库，设定查询语句
    if (model->rowCount()<0)//返回数据库的行数
    {
        qDebug()<<"model error !";
    }
    else
    {
        r = model->rowCount();
    }

    return r;
}

uint MySqlite::getRowCount(QString &sql)
{
    int count = 0;
    QSqlQuery query(m_db);

    bool flag = query.exec(sql);

    if(flag)
    {
        if(query.next())
        {
            count = query.value(0).toUInt();
            //qDebug()<<"count = "<<count;
        }
    }
    else
    {
        //qDebug()<<"MySqlite.cpp 277 error \n";
    }
    query.finish();
    query.clear();

    return count;
}

uint MySqlite::getRowCount()
{
    int count = 0;
    QString sql = "select count(*) from RECORD;";
    QSqlQuery query(m_db);

    bool flag = query.exec(sql);

    if(flag)
    {
        if(query.next())
        {
            count = query.value(0).toUInt();
        }
    }
    else
    {
        qDebug()<<"MySqlite.cpp 208 error \n";
    }
    query.finish();
    query.clear();

    return count;
}

bool MySqlite::delData(QString sql)
{
    bool flag;

    QSqlQuery query(m_db);
    m_db.transaction();
    if(query.exec(sql))
    {
        flag = true;
    }
    else
    {
        flag = false;
    }
    m_db.commit();
    query.finish();
    query.clear();
    return flag;
}

bool MySqlite::delTemp(int net, int id,int type)
{
    bool flag;
    QString netStr  = QString::number(net);
    QString idStr   = QString::number(id);
    QString typeStr = QString::number(type);

    QString sql;
    if(type == 0x05)
    {
        sql = "delete from TEMP_POWER where net = "+netStr+" and id = "+idStr+" and type = "+typeStr+";";
    }
    else
    {
        sql = "delete from TEMP_ERROR where net = "+netStr+" and id = "+idStr+" and type = "+typeStr+";";
    }
    QSqlQuery query(m_db);
    m_db.transaction();
    if(query.exec(sql))
    {
        flag = true;
    }
    else
    {
        flag = false;
    }
    m_db.commit();
    query.finish();
    query.clear();
    return flag;
}

void MySqlite::recoverPasswd()
{
    updateUserPasswd(MySqlite::USER,  QString("111111"));
    updateUserPasswd(MySqlite::ADMIN, QString("222222"));
}


uint MySqlite::getNodeNum(uint net, uint id)
{
    uint exsit = 0;

    QString sql = "select count(*) from NODE WHERE enable = 1 and net = "+QString::number(net)+" and id = "+QString::number(id)+";";
    //qDebug()<<"getNodeNum slq"<<sql;
    QSqlQuery query(m_db);

    if(query.exec(sql))
    {
        if(query.next())
        {
            exsit = query.value(0).toUInt();
        }
    }
    query.clear();
    return exsit;
}

void MySqlite::setPastTime(int time)
{
    QString sql;
    sql.clear();
    sql = "update SYSTEM set PASTTIME = "+QString::number(time)+",KEYFLAG = 1 where rowid = 1;";
    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();

}

int MySqlite::getPastTime()
{
    int pastTime = 0;
    QString sql = "select PASTTIME from SECRETKEY where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            pastTime = query.value(0).toULongLong();
        }
    }
    query.clear();
    return pastTime;
}


bool MySqlite::insertSecretKeyTime(int time,uint keyflag = 0)
{
    QString sql;
    bool flag = false;
    sql.clear();
    sql = "update SECRETKEY set TOTALTIME = "+QString::number(time)+",KEYFLAG = "+QString::number(keyflag)+" where rowid = 1;";
    QSqlQuery query(m_db);
    flag = query.exec(sql);
    query.clear();
    return flag;
}

bool MySqlite::insertHost(QString ip)
{
    QString sql = "update HOST set IP = '"+ip+"' where rowid = 1;";
    //qDebug()<<"sql : "<<sql;
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        return true;
    }
    return false;
}

QString MySqlite::getHost()
{
    QString ip;
    QString sql = "select IP from HOST;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            ip = query.value(0).toString();
        }
    }
    query.clear();

    //qDebug()<<"getHost : "<<ip;//jiang


    return ip;
}

bool MySqlite::stopSecretKeyTime()
{
    QString sql;
    bool flag = false;
    sql.clear();
    sql = "update SECRETKEY set TOTALTIME = 0,PASTTIME = 0, KEYFLAG = 0 ,KEYTIMEOUT = 0 where rowid = 1;";
    QSqlQuery query(m_db);
    flag = query.exec(sql);
    query.clear();
    return flag;
}

uint MySqlite::getTotleTime()
{
    qulonglong totelTime = 0;
    QString sql = "select TOTALTIME from SECRETKEY where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            totelTime  = query.value(0).toULongLong();
        }
    }
    query.clear();
    return totelTime;
}



//服务器相关jiang20190512
 QString MySqlite::getServerIPOrName(){
     QString ip_or_name;
     QString sql = "select ip_or_name from SERVER_INFO;";
     QSqlQuery query(m_db);
     if(query.exec(sql))
     {
         if(query.next())
         {
             ip_or_name = query.value(0).toString();
         }
     }
     query.clear();
     //qDebug()<<"ip_or_name : "<<ip_or_name;//jiang
     return ip_or_name;
 }
 uint MySqlite::getServerPort(){

     qulonglong server_port = 0;
     QString sql = "select server_port from SERVER_INFO;";
     QSqlQuery query(m_db);
     if(query.exec(sql))
     {
         if(query.next())
         {
             server_port  = query.value(0).toULongLong();
         }
     }
     query.clear();
     return server_port;

 }
 uint MySqlite::getDataInterval(){


     qulonglong data_interval = 0;
     QString sql = "select data_interval from SERVER_INFO;";
     QSqlQuery query(m_db);
     if(query.exec(sql))
     {
         if(query.next())
         {
             data_interval  = query.value(0).toULongLong();
         }
     }
     query.clear();
     return data_interval;
 }
 uint MySqlite::getHeartInterval(){



     qulonglong heart_interval = 0;
     QString sql = "select heart_interval from SERVER_INFO;";
     QSqlQuery query(m_db);
     if(query.exec(sql))
     {
         if(query.next())
         {
             heart_interval  = query.value(0).toULongLong();
         }
     }
     query.clear();
     return heart_interval;
 }
 QString MySqlite::getLocalDns(){

     QString local_dns;
     QString sql = "select local_dns from SERVER_INFO;";
     QSqlQuery query(m_db);
     if(query.exec(sql))
     {
         if(query.next())
         {
             local_dns = query.value(0).toString();
         }
     }
     query.clear();
     //qDebug()<<"local_dns : "<<local_dns;
     return local_dns;
 }

 bool MySqlite::setServerIPOrName(QString ip_or_name){
     bool isSuccess = false;
     QString sql = "update SERVER_INFO set ip_or_name = '"+ip_or_name+"';";
     QSqlQuery query(m_db);
     if(query.exec(sql))
     {
         isSuccess = true;
     }
     query.clear();
     return isSuccess;
 }
 bool MySqlite::setServerPort(uint server_port){

     QString sql;
     bool isSuccess = false;
     sql.clear();
     sql = "update SERVER_INFO set server_port = "+QString::number(server_port)+";";
     QSqlQuery query(m_db);
     if(query.exec(sql))
     {
         isSuccess = true;
     }
     query.clear();
     return isSuccess;
 }
 bool MySqlite::setDataInterval(uint data_interval){

     QString sql;
     bool isSuccess = false;
     sql.clear();
     sql = "update SERVER_INFO set data_interval = "+QString::number(data_interval)+";";
     QSqlQuery query(m_db);
     if(query.exec(sql))
     {
         isSuccess = true;
     }
     query.clear();
     return isSuccess;
 }
 bool MySqlite::setHeartInterval(uint heart_interval){

     QString sql;
     bool isSuccess = false;
     sql.clear();
     sql = "update SERVER_INFO set heart_interval = "+QString::number(heart_interval)+";";
     QSqlQuery query(m_db);
     if(query.exec(sql))
     {
         isSuccess = true;
     }
     query.clear();
     return isSuccess;

 }
 bool MySqlite::setLocalDns(QString local_dns){
     bool isSuccess = false;
     QString sql = "update SERVER_INFO set local_dns = '"+local_dns+"';";
     QSqlQuery query(m_db);
     if(query.exec(sql))
     {
         isSuccess = true;
     }
     query.clear();
     return isSuccess;
 }

 // 获得MAC地址
 QString MySqlite::getMACAdress()
 {
     QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
     if(nets.isEmpty()){
         return "00:00:01:00:00:01";
     }
     int nCnt = nets.count();
     if(nCnt<1){
         return "00:00:01:00:00:01";
     }
     QString strMacAddr = "";
     for(int i = 0; i < nCnt; i ++)
     {
         // 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址
         if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning)
                 && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
         {
             strMacAddr = nets[i].hardwareAddress();
             break;
         }
     }
     if(strMacAddr.isEmpty()){
         return "00:00:01:00:00:01";
     }
     qDebug()<<"strMacAddr"+strMacAddr;
     return strMacAddr;
 }

 QString MySqlite::getLocalMacFromDB()
 {

     QString local_mac;
     QString sql = "select m_mac from MAC_INFO;";
     QSqlQuery query(m_db);
     if(query.exec(sql))
     {
         if(query.next())
         {
             local_mac = query.value(0).toString();
         }
     }
     query.clear();
     //qDebug()<<"local_dns : "<<local_dns;
     return local_mac;
 }

 bool MySqlite::setLocalMacToDB(QString localMac)
 {
     bool isSuccess = false;
     //QString sql = "update SERVER_INFO set local_dns = '"+local_dns+"';";
     QString sql = "update MAC_INFO set m_mac = '"+localMac+"';";
     QSqlQuery query(m_db);
     if(query.exec(sql))
     {
         isSuccess = true;
     }
     query.clear();
     return isSuccess;
 }

