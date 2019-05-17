#ifndef HOSTINFO_H
#define HOSTINFO_H

#include <QWidget>
#include "MySqlite/mysqlite.h"
namespace Ui {
class HostInfo;
}

class HostInfo : public QWidget
{
    Q_OBJECT

public:
    explicit HostInfo(QWidget *parent = 0);
    ~HostInfo();
    void initVar();

private:
    Ui::HostInfo *ui;
    MySqlite m_db;
signals:
    void sigMainPageShow();
private slots:
    void slotTBtnQuit();
public slots:
    void slotHostInfoShow();
};

#endif // HOSTINFO_H
