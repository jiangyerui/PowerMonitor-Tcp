#ifndef DEVINFO_H
#define DEVINFO_H

#include <QMap>
#include <QTimer>
#include <QWidget>
#include <QDateTime>
#include <QScrollBar>
#include <QMessageBox>
#include <QButtonGroup>
#include <QTableWidget>
#include "Moudle/moudle.h"
#include "MySqlite/mysqlite.h"
#include "GlobalData/globaldata.h"

#define PAGECOUNT 10
namespace Ui {
class DevInfo;
}

class DevInfo : public QWidget
{
    Q_OBJECT

public:
    explicit DevInfo(QWidget *parent = 0);
    ~DevInfo();
    void initVar();
    void initStyle();
    void initDevNode(QMap<int, QList<int> > nodeMap);
    QString setModType(uint curNet, uint curId);//探测器类型
    void setModSts(uint curNet,uint curId);
    void initConnect();
    void initNodeBtn();//初始化节点按钮
    void lcdNumberClean();//清空数据
    void moduleStatus(int curNet, int curPage);
    int getCountPage(int curNet);
    void setNetShow(int netNum);
    void showModValue(uint curNet, uint curId);
    void setBtnVisible();

    void initTableWidget(QTableWidget *tableWidget);//初始化表格
    //显示数据表格
    void showTableData(QTableWidget *tableWidget, QSqlQueryModel*model, MySqlite &db, int type);

    QMap<int, QList<Moudle> > *m_moudleMap;

signals:
    void sigSoundType(int type);
    void sigSoundSW(bool soundSW);
    void sigResetShow();//复位
    void sigSetAlarmValue(int);//报警显示位置
    void sigSetErrorValue(int);//故障显示位置
    void sigErrorLed(bool enable);
    void sigMainPageShow();

private:
    Ui::DevInfo *ui;

    QTimer *m_currentTimer;
    QButtonGroup *m_btnGroup;
    int m_times;

    MySqlite m_db;
    QMap<int ,QList<int> > m_nodeMap;
    int m_nodeNum_1;
    int m_nodeNum_2;
    int m_curId;
    int m_curNet;//当前网络
    int m_curPage;//当前页
    int m_countPage;//页总数

    bool m_soundFlag;//声音开关
    int m_userType;//用户类型

    QString m_styleSheet;
    QString m_redStyle;
    QString m_greenStyle;
    QString m_yellowStyle;

    QString m_normalStyle;
    QString m_pressedStyle;


    int m_tabWTimes;
    int errorNum;
    int powerNum;
    int m_errorIndex;
    int m_alarmIndex;
    QSqlQueryModel *m_errorModel;
    QSqlQueryModel *m_alarmModel;
    QList<QTableWidgetItem *> m_itemAlarmList;
    QList<QTableWidgetItem *> m_itemErrorList;
private slots:
    void slotCurrentTime();


    void slotBtnClick(int index);//点击按钮
    void slotBtnPass_1();//网络切换1
    void slotBtnPass_2();//网络切换2
    void slotBtnJump();  //跳转到指定页
    void slotBtnLastPage(); //上一页
    void slotBtnNestPage(); //下一页

    void slotBtnErrorUp();  //向上查询故障记录
    void slotBtnErrorDown();//向下查询故障记录
    void slotBtnAlarmUp();  //向上查询报警记录
    void slotBtnAlarmDown();//向下查询报警记录

    void slotResetShow();//复位
    void slotBtnSound();//声音
public slots:
    void slotSound();
    void slotBtnBack();
    void slotDevInfoShow();
    void slotLoginStatus(int type);//登陆类型
    void slotPowerSts(bool sts);
};

#endif // DEVINFO_H
