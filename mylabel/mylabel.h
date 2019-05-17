#ifndef MYLABEL_H
#define MYLABEL_H

#include <QWidget>
#include <QLabel>
#include <QTimerEvent>
#include <QTimer>
#include <QPainter>

class MyLabel : public QLabel
{
    Q_OBJECT

public:
    explicit MyLabel(QWidget *parent = 0);
    void setText(const QString &newText);
    QString text() const { return mystr; }

    void myCompany();
protected:
    void paintEvent(QPaintEvent *event);

    void timerEvent(QTimerEvent *event);

    void showEvent(QShowEvent *event);






private:
    int offset;
    QString mystr;
    int myTimerId;
};

#endif // MYLABEL_H
