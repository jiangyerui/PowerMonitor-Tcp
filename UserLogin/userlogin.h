#ifndef USERLOGIN_H
#define USERLOGIN_H

#include <QWidget>
#include "MySqlite/mysqlite.h"
namespace Ui {
class UserLogin;
}

class UserLogin : public QWidget
{
    Q_OBJECT

public:
    explicit UserLogin(QWidget *parent = 0);
    ~UserLogin();
    void initVar();
     void showLock();

private:
    Ui::UserLogin *ui;
    MySqlite m_db;

    bool m_keyTimeOut;
    QString m_user;
    QString m_admin;
signals:
    void sigLoginStatus(int type);
    void sigMainPageShow();
    void sigSystem();
private slots:

    void slotBtnOk();
    void slotBtnCancel();
public slots:

    void slotLoginShow();//初始化登录窗口



};

#endif // USERLOGIN_H
