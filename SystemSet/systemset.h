#ifndef SYSTEMSET_H
#define SYSTEMSET_H

#include <QWidget>
#include <time.h>
#include <QDebug>
#include <QMessageBox>
#include "MySqlite/mysqlite.h"
#include "GlobalData/globaldata.h"

namespace Ui {
class SystemSet;
}

class SystemSet : public QWidget
{
    Q_OBJECT

public:
    explicit SystemSet(QWidget *parent = 0);
    ~SystemSet();
    void initQT();
    void initVar();
    void initConnect();
    void setPrintType(bool type);
    void setSerialNum(QString serialNum);

signals:
    void sigShowLock();

private:
    Ui::SystemSet *ui;
    MySqlite m_db;
    int m_userType;
    QString m_serialNum;

    QTimer *m_keyTimer;
    bool m_keyLock;//是否解锁
    bool m_keyFlag;//是否开启
    int m_keyTotalNum;//时间数目
    bool m_keyTimeOver;//时间是否超过
private slots:

    void slotBtnClose();//返回
    void slotBtnSysTime();//设置系统时间
    void slotBtnSystemSet();//系统配置
    void slotBtnSerialNum();//设置序列号
    void slotBtnPrintType();//设置打印类型
    void slotBtnUpdatePasswd();//更新密码
    void slotBtnRecoverPasswd();//恢复密码
    void slotBtnModRead();
    void slotBtnModWrite();

    void slotKeyTimeOut();
    void slotBtnKeyStop();
    void slotBtnKeyStart();

    void slotBtnSetHost();
    void on_pBtn_serverSet_clicked();

public slots:
    void slotSystemShow();
    void slotLoginStatus(int type);//登陆类型


};

#endif // SYSTEMSET_H
