#include "mylabel.h"

MyLabel::MyLabel(QWidget *parent) :
    QLabel(parent)
{

}

void MyLabel::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter painter(this);

    ++offset;
    if (offset >= fontMetrics().width(text()))
    {
        offset = 0;
    }
    scroll(-1, -1);

    int textWidth = fontMetrics().width(text());
    if (textWidth < 1)
        return;
    int x = -offset;
    while (x <textWidth) {
        painter.drawText(x, 0, textWidth, height(),Qt::AlignLeft | Qt::AlignVCenter,text());
        x += textWidth;
    }
}

void MyLabel::showEvent(QShowEvent * /* event */)

{
    myTimerId = startTimer(1000);
}

void MyLabel::timerEvent(QTimerEvent *event)

{

    if (event->timerId() == myTimerId) {
        ++offset;
        if (offset >= fontMetrics().width(text()))
            offset = 0;
        scroll(-1, 0);
    }
    else
    {
        QWidget::timerEvent(event);
    }
}

void MyLabel::setText(const QString &newText)
{
    mystr = newText;
    update();
    updateGeometry();

}


void MyLabel::myCompany()
{
   setFont(QFont("文泉驿等宽微米黑",14));

   setGeometry(10,440,780,30);
   setAlignment(Qt::AlignHCenter);
   setStyleSheet("color:rgb(255, 255, 255);");
   setText(tr("   公司简介：  公司主要产品包括智能消防应急照明和疏散指示系统、 电气火灾监控系统、 防火门监控系统、 "
              "消防设备电源状态监控系统、 数字智能消防巡检柜、 故障电弧探测器、 防火限流保护器、 热解粒子探测器等。  "
              "地  址：  北京市通州区聚富苑聚富南路8号         "
              "邮  编：  101105            "
              "电  话：  010-80597528       "
              "邮  箱：  shangwubu@holianda.com "));

}
