#-------------------------------------------------
#
# Project created by QtCreator 2017-09-04T14:50:07
#
#-------------------------------------------------


QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = PowerMonitor
DESTDIR = destApp/
TEMPLATE = app
CONFIG += serialport


DEFINES += RUN_ON_ARM

contains(DEFINES,RUN_ON_ARM){

SOURCES += main.cpp\
        mainwindow.cpp \
    DevInfo/devinfo.cpp \
    HostInfo/hostinfo.cpp \
    Record/record.cpp \
    UserLogin/userlogin.cpp \
    SystemSet/systemset.cpp \
    MySqlite/mysqlite.cpp \
    Manager/manager.cpp \
    MyPrint/myprint.cpp \
#jaing
    LinuxCom/posix_qextserialport.cpp \
    LinuxCom/qextserialbase.cpp \
#jiang
    Moudle/moudle.cpp \
    CanMoudle/canmoudle.cpp \
    SelfCheck/screencheck.cpp \
    GpioChip/gpiochip.cpp \
    GpioControl/gpiocontrol.cpp \
    GlobalData/globaldata.cpp \
    Keyboard/Keyboard.cpp \
    Imframe/Imframe.cpp \
    ScreenSaver/screensaver.cpp

HEADERS  += mainwindow.h \
    DevInfo/devinfo.h \
    HostInfo/hostinfo.h \
    Record/record.h \
    UserLogin/userlogin.h \
    SystemSet/systemset.h \
    MySqlite/mysqlite.h \
    Manager/manager.h \
    MyPrint/myprint.h \
#jiang
    LinuxCom/posix_qextserialport.h \
    LinuxCom/qextserialbase.h \
#jiang
    Moudle/moudle.h \
    CanMoudle/canmoudle.h \
    SelfCheck/screencheck.h \
    GpioChip/gpiochip.h \
    GpioControl/gpiocontrol.h \
    GlobalData/globaldata.h \
    Keyboard/Keyboard.h \
    Imframe/Imframe.h \
    ScreenSaver/screensaver.h

FORMS    += mainwindow.ui \
    DevInfo/devinfo.ui \
    HostInfo/hostinfo.ui \
    Record/record.ui \
    UserLogin/userlogin.ui \
    SystemSet/systemset.ui \
    SelfCheck/screencheck.ui \
    Keyboard/Keyboard.ui

}
else{
SOURCES += main.cpp\
        mainwindow.cpp \
    DevInfo/devinfo.cpp \
    HostInfo/hostinfo.cpp \
    Record/record.cpp \
    UserLogin/userlogin.cpp \
    SystemSet/systemset.cpp \
    MySqlite/mysqlite.cpp \
    Manager/manager.cpp \
    MyPrint/myprint.cpp \
    Moudle/moudle.cpp \
    CanMoudle/canmoudle.cpp \
    SelfCheck/screencheck.cpp \
    GpioChip/gpiochip.cpp \
    GpioControl/gpiocontrol.cpp \
    GlobalData/globaldata.cpp \

HEADERS  += mainwindow.h \
    DevInfo/devinfo.h \
    HostInfo/hostinfo.h \
    Record/record.h \
    UserLogin/userlogin.h \
    SystemSet/systemset.h \
    MySqlite/mysqlite.h \
    Manager/manager.h \
    MyPrint/myprint.h \
    Moudle/moudle.h \
    CanMoudle/canmoudle.h \
    SelfCheck/screencheck.h \
    GpioChip/gpiochip.h \
    GpioControl/gpiocontrol.h \
    GlobalData/globaldata.h \


FORMS    += mainwindow.ui \
    DevInfo/devinfo.ui \
    HostInfo/hostinfo.ui \
    Record/record.ui \
    UserLogin/userlogin.ui \
    SystemSet/systemset.ui \
    SelfCheck/screencheck.ui \
}



RESOURCES += \
    image.qrc

HEADERS += \
    watchdog/watchdog.h \
    TcpServer/tcpserver.h \
    udpclient.h

SOURCES += \
    watchdog/watchdog.cpp \
    TcpServer/tcpserver.cpp \
    udpclient.cpp

FORMS +=


