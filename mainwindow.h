#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>


#include "DevInfo/devinfo.h"
#include "HostInfo/hostinfo.h"
#include "Record/record.h"
#include "DevInfo/devinfo.h"
#include "UserLogin/userlogin.h"
#include "SystemSet/systemset.h"
#include <QTimer>
//#define TIMEOUT   30 //看门狗时间//jiang20190512
#define TIMEOUT   600




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initVar();
    void initConnect();
    void HideAllTopWidget();

signals:
    void sigBtnUser();
    void sigBtnRecord();
    void sigBtnDevInfo();
    void sigBtnHostSet();
    void sigBtnHostInfo();
    void sigBtnHostCheck();

    void sigLoginStatus(int type);

private:
    Ui::MainWindow *ui;
    QTimer *m_timer;
public slots:
    void slotBtnQuit();
    void slotTimeOut();
    void slotShowMain();
    void slotLoginStatus(int type);//登陆类型








};

#endif // MAINWINDOW_H
