#ifndef RECORD_H
#define RECORD_H

#include <QTableWidget>
#include <QWidget>
#include <QMessageBox>
#include "MySqlite/mysqlite.h"
#include "MyPrint/myprint.h"
#include "GlobalData/globaldata.h"


namespace Ui {
class Record;
}

class Record : public QWidget
{
    Q_OBJECT

public:
    explicit Record(QWidget *parent = 0);
    ~Record();
    void initVar();
    void initConnect();
    void initTableWidget(QTableWidget *tableWidget);
    void setBtnEnable(bool enabel,int current,int maxPage);

    void delAllData();
    int getPageCount(QString sql);
    void delRowData(QString netStr, QString idStr, QString timeStr);
    void showData(QTableWidget *tableWidget, QString sql, int currentPage);

    void setPrint(QTableWidget *tableWidget, MyPrint *myPrint, int row, int columnCount);
    QString setSelectSql(int item, bool net, bool id,QString &startTimer,QString &endTime);

private:
    Ui::Record *ui;

    MyPrint *m_myPrint;
    QSqlQueryModel *m_model;
    QList<QTableWidgetItem *> m_itemList;



    MySqlite m_db;
    int m_maxPage;
    int m_userType;
    int m_currentPage;


private slots:

    void slotBtnJump();
    void slotBtnPrint();
    void slotBtnSearch();
    void slotBtnUpPage();
    void slotBtnDelData();
    void slotBtnDownPage();
    void slotSetIdEnable(bool enable);
    void slotSetNetEnable(bool enable);

public slots:
    void slotRecordShow();
    void slotLoginStatus(int type);//登陆类型


};

#endif // RECORD_H
