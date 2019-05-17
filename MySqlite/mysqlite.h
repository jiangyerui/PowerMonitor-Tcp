#ifndef MYSQLITE_H
#define MYSQLITE_H

#include <QtSql>
#include <QFile>
#include <QString>
#include <QMutex>
#include <QDebug>
#include <QObject>
#include <QVariant>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QNetworkInterface>

/*    主要功能
 * 1.初始化,链接数据库
 * 2.插入临时数据据
 * 3.获取主机序列号
 * 4.设置主机序列号
 * 5.获取序列号密钥
 * 6.获取密钥状态
 * 7.获取加锁状态
 * 8.获取节点地址
 * 9.获取打印配置
 *10.设置打印模式
 *11.获取用户密码
 *12.更新用户密码
 *13.获取轮询时间
 *14.设置轮询时间
 *15.获取总共时间
 *16.停止密钥时间
 *17.插入密钥启动总时间
 *18.获取数据行总数-1
 *19.获取数据行总数-2
 *20.获取数据行总数-3
 *21.删除数据
 *22.删除临时数据
 *23.恢复密码
 *24.设置未解锁
 *25.插入曲线数据
 *26.更新曲线数据
 *27.获取曲线参数值
 *28.
*/

class MySqlite : public QObject
{
    Q_OBJECT
public:
    explicit MySqlite(QObject *parent = 0);

    enum USERTYPE{
        USER  = 1,
        ADMIN = 2,
        QUIT  = 3

    };

    void initDb();//初始化,链接数据库
    void insertNode(int net, int id, QString address);//插入临时数据据
    void insertTemp(int net, int id, int type, uint time);//插入临时数据据
    void insertAlarm(int net, int id, int status, uint time, QString address);//插入报警数据


    QString getNodeAddress(int net,int id);//获取节点地址
    uint getModRatio(int net,int id);
    bool setModRatio(int net,int id,uint ratio);


    uint getPollTime();//获取轮询时间
    bool setPollTime(QString time);//设置轮询时间

    uint getRowCount(QString &sql,QSqlQueryModel *model);//获取数据行总数-1
    uint getRowCount(QString &sql);//获取数据行总数-2
    uint getRowCount();//获取数据行总数-3

    bool delData(QString sql);//删除数据
    bool delTemp(int net, int id, int type);//删除临时数据
    void recoverPasswd();//恢复密码

    uint getNodeNum(uint net , uint id);

    //systemSet
    bool getPrintAlarm();
    void setPrintAlarm(bool model);

    bool getPrintError();
    void setPrintError(bool model);

    bool getPrintStyle();//获取打印配置
    void setPrintStyle(bool model);//设置打印模式

    QString getSerialNumber();//获取主机序列号
    bool insertSerialNumber(QString serialNO);//设置主机序列号

    QString getUserPasswd(int userType);//获取用户密码
    bool updateUserPasswd(int userType, QString passwd);//更新用户密码
    //record

    bool setSecretKey(QString secretKey);
    QString getSecretKey();
    bool getKeyFlag();
    bool getKeyTimeOut();
    bool setKeyTimeOut(int timeFlag);

    int getPastTime();//获取已过时间
    void setPastTime(int time);//设置已过时间

    uint getTotleTime();//获取总共时间
    bool stopSecretKeyTime();//停止密钥时间
    bool insertSecretKeyTime(int time, uint secretkeyflag);//插入密钥启动总时间


    bool insertHost(QString ip);
    QString getHost();


   //服务器相关jiang20190512
    QString getServerIPOrName();
    uint getServerPort();
    uint getDataInterval();
    uint getHeartInterval();
    QString getLocalDns();

    bool setServerIPOrName(QString ip_or_name);
    bool setServerPort(uint server_port);
    bool setDataInterval(uint data_interval);
    bool setHeartInterval(uint heart_interval);
    bool setLocalDns(QString local_dns);

    //MAC相关jiang20190516
    QString getMACAdress();
    QString getLocalMacFromDB();
    bool setLocalMacToDB(QString localMac);


private:
    QSqlDatabase m_db;

signals:

public slots:
};

#endif // MYSQLITE_H
