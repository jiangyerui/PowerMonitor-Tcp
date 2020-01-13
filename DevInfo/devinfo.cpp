#include "devinfo.h"
#include "ui_devinfo.h"

#define ERRORTIMES 1
#define POWERTIMES 2
DevInfo::DevInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DevInfo)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0,0,800,480);
    initVar();
    initStyle();
    initNodeBtn();
    initConnect();
    lcdNumberClean();
    //隐藏探测器地址界面
    ui->label_29->hide();
    ui->lb_address->hide();
}

DevInfo::~DevInfo()
{
    delete ui;
}

void DevInfo::initVar()
{
    m_times = 0;
    m_curNet = 1;
    m_curId   = 0;
    m_curPage = 1;
    m_nodeNum_1 = 0;
    m_nodeNum_2 = 0;
    m_countPage = 0;
    m_errorIndex = 0;
    m_alarmIndex = 0;
    m_tabWTimes = 1;

    errorNum = 0;
    powerNum = 0;
    m_nodeMap.clear();

    //ui->lb_page->hide();
    //ui->lineEdit->hide();
    //ui->tBtn_jump->hide();

    ui->tBtn_reset->setEnabled(false);
    ui->tBtn_voice->setEnabled(false);

    int netNum = m_db.getSerialNumber().mid(2,1).toInt();
    setNetShow(netNum);

    for(int net = 1;net <= netNum;net++)
    {
        QList<int> nodeList;nodeList.clear();
        for(uint id = 1;id <= NODENUM;id++)
        {
            if(m_db.getNodeNum(net,id))
            {
                g_mod[net][id].used = true;
                g_mod[net][id].normalFlag = true;
                g_mod[net][id].id   = id;
                g_mod[net][id].net  = net;
                nodeList.append(id);
            }
        }
        m_nodeMap.insert(net,nodeList);
    }
}

void DevInfo::initStyle()
{

    m_redStyle   = "QPushButton{border:1px solid;border-radius:10px; color: rgb(0, 0, 0);"
                   "font: 20pt '文泉驿等宽微米黑'; ""border-color: rgb(53, 156, 212);"
                   "background-color: rgb(255, 0, 0);}"
                   "QPushButton:focus{outline: none; color: rgb(85, 170, 255);}";

    m_greenStyle = "QPushButton{border:1px solid;""border-radius:10px;color: rgb(0, 0, 0);"
                   "font: 20pt '文泉驿等宽微米黑';border-color: rgb(53, 156, 212);"
                   "background-color: rgb(0, 255, 0);}"
                   "QPushButton:focus{outline: none; color: rgb(85, 170, 255);}";

    m_yellowStyle= "QPushButton{border:1px solid;border-radius:10px;color: rgb(0, 0, 0);"
                   "font: 20pt '文泉驿等宽微米黑';border-color: rgb(53, 156, 212);"
                   "background-color: rgb(255, 255, 0);}"
                   "QPushButton:focus{outline: none; color: rgb(85, 170, 255);}";

    m_normalStyle  = "border:1px solid;border-radius:8px; color: rgb(255, 255, 255);border-color:rgb(53, 156, 212);";
    m_pressedStyle = "border:1px solid;border-radius:8px; color: rgb(0 ,0, 0);background-color:rgb(85, 170, 255);";

    m_alarmModel = new QSqlQueryModel();
    m_errorModel = new QSqlQueryModel();

    //初始化故障,报警显示
    initTableWidget(ui->tabWid_Alarm);
    initTableWidget(ui->tabWid_Error);

    ui->tBtn_pass1->setStyleSheet(m_pressedStyle);
    ui->tBtn_pass2->setStyleSheet(m_normalStyle);

    //设置跳转页面只能输入大于0<value<26
    QRegExp regExp("^([1-9])|(1[0-9])|(2[0-6])$");
    ui->lineEdit->setValidator(new QRegExpValidator(regExp, this));
    ui->lineEdit->clear();
}

void DevInfo::initDevNode(QMap<int ,QList<int> > nodeMap)
{
    m_nodeMap = nodeMap;
}

QString DevInfo::setModType(uint curNet,uint curId)
{

    QString typeStr;
    switch (g_mod[curNet][curId].type) {
    case MOD_NONE:
        typeStr = tr("类型数据未上传");
        break;
    case MOD_V3:
        typeStr = tr("双路三相电压");
        break;
    case MOD_V:
        typeStr = tr("六路单相电压");
        break;
    case MOD_VA3:
        typeStr = tr("电压电流型");
        break;
    case MOD_DVA:
        typeStr = tr("直流电压电流");
        break;
    case MOD_VA:
        typeStr = tr("单相电压电流");
        break;
    case MOD_VN3:
        typeStr = tr("三相双路有零");
        break;
    case MOD_VAN3:
        typeStr = tr("电压电流有零");
        break;
    case MOD_2VAN3:
        typeStr = tr("两路三相电压一路三相电流");
        break;
    default:
        break;
    }
    return typeStr;
}

void DevInfo::setModSts(uint curNet, uint curId)
{

    if(g_mod[curNet][curId].id == curId && g_mod[curNet][curId].net == curNet)
    {
        if(g_mod[curNet][curId].normalFlag)
        {
            ui->lb_status->setText(tr("正常"));
        }
        else if(g_mod[curNet][curId].dropFlag)
        {
            ui->lb_status->setText(tr("通讯故障"));
        }
        else if(g_mod[curNet][curId].overCurrentFlag)
        {
            ui->lb_status->setText(tr("过流"));
        }
        else if(g_mod[curNet][curId].overVoltageFlag)
        {
            ui->lb_status->setText(tr("过压"));
        }
        else if(g_mod[curNet][curId].underVoltageFlag)
        {
            ui->lb_status->setText(tr("欠压"));
        }
        else if(g_mod[curNet][curId].phaseLossFlag)
        {
            ui->lb_status->setText(tr("缺相"));
        }
        else if(g_mod[curNet][curId].interruptionFlag)
        {
            ui->lb_status->setText(tr("供电中断"));
        }
        showModValue(curNet, curId);
    }
}


void DevInfo::initConnect()
{
    connect(ui->tBtn_back,SIGNAL(clicked(bool)),this,SLOT(slotBtnBack()));
    connect(ui->tBtn_voice,SIGNAL(clicked(bool)),this,SLOT(slotBtnSound()));
    connect(ui->tBtn_reset,SIGNAL(clicked(bool)),this,SLOT(slotResetShow()));

    m_currentTimer = new QTimer;
    connect(m_currentTimer,SIGNAL(timeout()),this,SLOT(slotCurrentTime()));
    m_currentTimer->start(1000);
    QTimer::singleShot(3000,this,SLOT(slotBtnPass_1()));

    //connect(ui->tBtn_jump,SIGNAL(clicked(bool)),this,SLOT(slotBtnJump()));
    connect(ui->tBtn_pass1,SIGNAL(clicked(bool)),this,SLOT(slotBtnPass_1()));
    connect(ui->tBtn_pass2,SIGNAL(clicked(bool)),this,SLOT(slotBtnPass_2()));
    connect(ui->tBtn_lastPage,SIGNAL(clicked(bool)),this,SLOT(slotBtnLastPage()));
    connect(ui->tBtn_nestPage,SIGNAL(clicked(bool)),this,SLOT(slotBtnNestPage()));

    connect(ui->tBtn_downError,SIGNAL(clicked(bool)),this,SLOT(slotBtnErrorDown()));
    connect(ui->tBtn_upError,SIGNAL(clicked(bool)),this,SLOT(slotBtnErrorUp()));
    connect(this,SIGNAL(sigSetErrorValue(int)),ui->tabWid_Error->verticalScrollBar(),SLOT(setValue(int)));

    connect(ui->tBtn_downAlarm,SIGNAL(clicked(bool)),this,SLOT(slotBtnAlarmDown()));
    connect(ui->tBtn_upAlarm,SIGNAL(clicked(bool)),this,SLOT(slotBtnAlarmUp()));
    connect(this,SIGNAL(sigSetAlarmValue(int)),ui->tabWid_Alarm->verticalScrollBar(),SLOT(setValue(int)));


}

void DevInfo::initNodeBtn()
{
    m_btnGroup = new QButtonGroup;
    m_btnGroup->addButton(ui->pBtn_1, 1);
    m_btnGroup->addButton(ui->pBtn_2, 2);
    m_btnGroup->addButton(ui->pBtn_3, 3);
    m_btnGroup->addButton(ui->pBtn_4, 4);
    m_btnGroup->addButton(ui->pBtn_5, 5);
    m_btnGroup->addButton(ui->pBtn_6, 6);
    m_btnGroup->addButton(ui->pBtn_7, 7);
    m_btnGroup->addButton(ui->pBtn_8, 8);
    m_btnGroup->addButton(ui->pBtn_9, 9);
    m_btnGroup->addButton(ui->pBtn_10,10);
    m_btnGroup->addButton(ui->pBtn_11,11);
    m_btnGroup->addButton(ui->pBtn_12,12);
    m_btnGroup->addButton(ui->pBtn_13,13);
    m_btnGroup->addButton(ui->pBtn_14,14);
    m_btnGroup->addButton(ui->pBtn_15,15);
    m_btnGroup->addButton(ui->pBtn_16,16);
    m_btnGroup->addButton(ui->pBtn_17,17);
    m_btnGroup->addButton(ui->pBtn_18,18);
    m_btnGroup->addButton(ui->pBtn_19,19);
    m_btnGroup->addButton(ui->pBtn_20,20);
    m_btnGroup->addButton(ui->pBtn_21,21);
    m_btnGroup->addButton(ui->pBtn_22,22);
    m_btnGroup->addButton(ui->pBtn_23,23);
    m_btnGroup->addButton(ui->pBtn_24,24);
    m_btnGroup->addButton(ui->pBtn_25,25);
    m_btnGroup->addButton(ui->pBtn_26,26);
    m_btnGroup->addButton(ui->pBtn_27,27);
    m_btnGroup->addButton(ui->pBtn_28,28);
    m_btnGroup->addButton(ui->pBtn_29,29);
    m_btnGroup->addButton(ui->pBtn_30,30);
    m_btnGroup->addButton(ui->pBtn_31,31);
    m_btnGroup->addButton(ui->pBtn_32,32);
    m_btnGroup->addButton(ui->pBtn_33,33);
    m_btnGroup->addButton(ui->pBtn_34,34);
    m_btnGroup->addButton(ui->pBtn_35,35);
    m_btnGroup->addButton(ui->pBtn_36,36);
    m_btnGroup->addButton(ui->pBtn_37,37);
    m_btnGroup->addButton(ui->pBtn_38,38);
    m_btnGroup->addButton(ui->pBtn_39,39);
    m_btnGroup->addButton(ui->pBtn_40,40);

    for(int i = 1;i<= NODEBASE;i++)
    {
        m_btnGroup->button(i)->setFocusPolicy(Qt::ClickFocus);
        m_btnGroup->button(i)->setVisible(false);
        m_btnGroup->button(i)->setStyleSheet(m_styleSheet);
    }

    connect(m_btnGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotBtnClick(int)));
}

void DevInfo::lcdNumberClean()
{

    ui->lb_type->clear();
    ui->lb_status->clear();
    ui->lb_address->clear();

    ui->lcdNumber_AI_1->display("0");
    ui->lcdNumber_BI_2->display("0");
    ui->lcdNumber_CI_3->display("0");
    ui->lcdNumber_AV_1->display("0");
    ui->lcdNumber_BV_2->display("0");
    ui->lcdNumber_CV_3->display("0");
    ui->lcdNumber_AV_4->display("0");
    ui->lcdNumber_BV_5->display("0");
    ui->lcdNumber_CV_6->display("0");
}

void DevInfo::moduleStatus(int curNet,int curPage)
{
    //显示节点个数
    int i = (curPage - 1 ) * NODEBASE;
    for(int index = 1;index <= NODEBASE;index++)
    {
        if( i < m_nodeMap.value(curNet).count())
        {
            int id = m_nodeMap.value(curNet).at(i);
            QString text = QString::number(id);

            m_btnGroup->button(index)->setText(text);
            m_btnGroup->button(index)->setVisible(true);

            if(g_mod[curNet][id].normalFlag == true)
            {
                m_btnGroup->button(index)->setStyleSheet(m_greenStyle);
            }
            else if(g_mod[curNet][id].overVoltageFlag  == true || g_mod[curNet][id].phaseLossFlag    == true ||
                    g_mod[curNet][id].underVoltageFlag == true || g_mod[curNet][id].interruptionFlag == true ||
                    g_mod[curNet][id].dropFlag == true)
            {
                m_btnGroup->button(index)->setStyleSheet(m_yellowStyle);
            }
            else if(g_mod[curNet][id].overCurrentFlag == true )
            {
                m_btnGroup->button(index)->setStyleSheet(m_redStyle);
            }
            i++;
        }
    }
}

int DevInfo::getCountPage(int curNet)
{
    //显示节点个数
    int regNum = m_nodeMap.value(curNet).count();
    QString str_1 = tr("节点 共");
    QString str_2 = tr("个");
    QString text  = str_1+QString::number(regNum)+str_2;
    ui->groupBox_moudel->setTitle(text);

    //显示页数
    int countPage;
    if(regNum <= NODEBASE)
    {
        countPage = 1;
    }
    else
    {
        for(int i = 1;i < PAGECOUNT;i++)
        {
            if((regNum > NODEBASE*i) && ( regNum <= NODEBASE*(i+1) ))
            {
                countPage = i+1;
            }
        }
    }
    return countPage;
}

void DevInfo::setNetShow(int netNum)
{
    if(netNum == 1)
    {
        ui->tBtn_pass2->hide();
    }
    else
    {
        ui->tBtn_pass2->show();
    }
}

void DevInfo::showModValue(uint curNet, uint curId)
{
    switch (g_mod[curNet][curId].type) {//modType
    case MOD_V:
    case MOD_V3:
    case MOD_VN3://三项双路有零

        ui->lcdNumber_AV_1->display(QString::number(g_mod[curNet][curId].AV_1));
        ui->lcdNumber_BV_2->display(QString::number(g_mod[curNet][curId].BV_2));
        ui->lcdNumber_CV_3->display(QString::number(g_mod[curNet][curId].CV_3));

        ui->lcdNumber_AV_4->display(QString::number(g_mod[curNet][curId].AV_4));
        ui->lcdNumber_BV_5->display(QString::number(g_mod[curNet][curId].BV_5));
        ui->lcdNumber_CV_6->display(QString::number(g_mod[curNet][curId].CV_6));

        ui->lcdNumber_AI_1->display(QString::number(0));
        ui->lcdNumber_BI_2->display(QString::number(0));
        ui->lcdNumber_CI_3->display(QString::number(0));

        break;
    case MOD_VA:
    case MOD_VA3://电压电流有零
    case MOD_VAN3://电压电流有零

        ui->lcdNumber_AV_1->display(QString::number(g_mod[curNet][curId].AV_1));
        ui->lcdNumber_BV_2->display(QString::number(g_mod[curNet][curId].BV_2));
        ui->lcdNumber_CV_3->display(QString::number(g_mod[curNet][curId].CV_3));
        ui->lcdNumber_AV_4->display(QString::number(0));
        ui->lcdNumber_BV_5->display(QString::number(0));
        ui->lcdNumber_CV_6->display(QString::number(0));
        ui->lcdNumber_AI_1->display(QString::number(g_mod[curNet][curId].AI_1 * g_mod[curNet][curId].IR));
        ui->lcdNumber_BI_2->display(QString::number(g_mod[curNet][curId].BI_2 * g_mod[curNet][curId].IR));
        ui->lcdNumber_CI_3->display(QString::number(g_mod[curNet][curId].CI_3 * g_mod[curNet][curId].IR));

        break;
    case MOD_2VAN3://两路三项电压一路三项电流

        ui->lcdNumber_AV_1->display(QString::number(g_mod[curNet][curId].AV_1));
        ui->lcdNumber_BV_2->display(QString::number(g_mod[curNet][curId].BV_2));
        ui->lcdNumber_CV_3->display(QString::number(g_mod[curNet][curId].CV_3));
        ui->lcdNumber_AV_4->display(QString::number(g_mod[curNet][curId].AV_4));
        ui->lcdNumber_BV_5->display(QString::number(g_mod[curNet][curId].BV_5));
        ui->lcdNumber_CV_6->display(QString::number(g_mod[curNet][curId].CV_6));
        ui->lcdNumber_AI_1->display(QString::number(g_mod[curNet][curId].AI_1 * g_mod[curNet][curId].IR));
        ui->lcdNumber_BI_2->display(QString::number(g_mod[curNet][curId].BI_2 * g_mod[curNet][curId].IR));
        ui->lcdNumber_CI_3->display(QString::number(g_mod[curNet][curId].CI_3 * g_mod[curNet][curId].IR));

        break;
    default:
        break;
    }
}

void DevInfo::setBtnVisible()
{
    for(int i = 1;i<= NODEBASE;i++)
    {
        m_btnGroup->button(i)->setVisible(false);
        m_btnGroup->button(i)->setText("");
    }

}

void DevInfo::initTableWidget(QTableWidget *tableWidget)
{
    QStringList alrmHeadList;
    tableWidget->setColumnCount(3);
    alrmHeadList<<tr("通  道")<<tr("节  点")<<tr("时  间");
    tableWidget->setHorizontalHeaderLabels(alrmHeadList);
    tableWidget->horizontalHeader()->setFixedHeight(20);
    tableWidget->setColumnWidth(0,50);
    tableWidget->setColumnWidth(1,60);
    tableWidget->setColumnWidth(2,200);

    QString horStyle = "QHeaderView::section{""background-color:rgb(255,255,255);"
                       "color: black;border: 1px solid #6c6c6c;}";;
    QString verStyle = "QHeaderView::section {""background-color:rgb(255,255,255);"
                       "color: black;border: 1px solid #6c6c6c; padding-left: 4px;}";
    tableWidget->horizontalHeader()->setStyleSheet(horStyle);
    tableWidget->verticalHeader()->setStyleSheet(verStyle);
    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setEnabled(false);
    tableWidget->horizontalHeader()->setVisible(true);//设置表头显示
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setHighlightSections(false);
    tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    tableWidget->setSelectionMode (QAbstractItemView::NoSelection); //设置选择模式，选择单行
}

void DevInfo::showTableData(QTableWidget *tableWidget, QSqlQueryModel *model, MySqlite &db, int type)
{
    QString sql ;
    int countNum = 0;
    if(type == ERRORDATA)
    {
        //error
        sql ="select NET,ID,TIME FROM TEMP_ERROR WHERE type != 5 ORDER BY TIME DESC;";
        countNum = db.getRowCount(sql,model);//返回故障数据的总行数
        errorNum = countNum;
        QString titleNum = tr("故障: ")+QString::number(countNum)+tr("个");
        ui->groupBox_Error->setTitle(titleNum);

//        qDeleteAll(m_itemErrorList.begin(),m_itemErrorList.end());
//        m_itemErrorList.clear();
    }
    else
    {
        //alarm
        sql ="select NET,ID,TIME FROM TEMP_POWER WHERE TYPE = 5 ORDER BY TIME DESC;";
        countNum = db.getRowCount(sql,model);//返回报警数据的总行数
        powerNum = countNum;
        QString titleNum = tr("供电中断: ")+QString::number(countNum)+tr("个");
        ui->groupBox_Alarm->setTitle(titleNum);

//        qDeleteAll(m_itemAlarmList.begin(),m_itemAlarmList.end());
//        m_itemAlarmList.clear();
    }

    tableWidget->clearContents();
    tableWidget->setRowCount(countNum);//设置行数

    if(countNum != 0)
    {
        QSqlRecord record;
        QFont font = QApplication::font();
        font.setPointSize(14);

        for(int row = 0; row < countNum; row++)
        {
            QTableWidgetItem *item;
            record = model->record(row);
            tableWidget->setRowHeight(row,60);
            for(int column = 0; column < 3; column++)
            {

                item = new QTableWidgetItem;
//                if(type == ERRORDATA)
//                {
//                    m_itemErrorList.append(item);
//                }
//                else
//                {
//                    m_itemAlarmList.append(item);
//                }
                item->setFont(font);
                item->setTextAlignment(Qt::AlignCenter);
                item->setTextColor(QColor(0,0,0));

                if(column == 0)
                {
                    QString netStr = record.value(0).toString();
                    item->setText(netStr);
                    tableWidget->setItem(row, column, item);
                }
                else if(column == 1)
                {
                    QString idStr = record.value(1).toString();
                    item->setText(idStr);
                    tableWidget->setItem(row, column, item);
                }
                else if(column == 2)
                {
                    uint dt = record.value(2).toString().toUInt();
                    QDateTime datetime;
                    datetime.setTime_t(dt);
                    QString dtstr=datetime.toString("yyyy-MM-dd/hh:mm:ss");
                    item->setText(dtstr);
                    tableWidget->setItem(row, column, item);
                }
            }
        }
    }
}

void DevInfo::slotCurrentTime()
{
    //显示当前时间
    QDateTime dateTime=QDateTime::currentDateTime();
    QString dtstr=dateTime.toString("yyyy-MM-dd/hh:mm:ss");
    ui->lb_time->setText(dtstr);


    if(m_tabWTimes == ERRORTIMES)
    {
        m_tabWTimes = 2;
        showTableData(ui->tabWid_Error,m_errorModel,m_db,ERRORDATA);
    }
    else if(m_tabWTimes == POWERTIMES)
    {
        m_tabWTimes = 1;
        showTableData(ui->tabWid_Alarm,m_alarmModel,m_db,INTERDATA);
    }

    //节点状态
    setModSts(m_curNet,m_curId);
    moduleStatus(m_curNet,m_curPage);

    //::system("free");
}



void DevInfo::slotBtnClick(int index)
{

    lcdNumberClean();
    QString currentNodeStr = m_btnGroup->button(index)->text();

    //当前节点
    ui->groupBox_SensorInfo->setTitle(tr("节点 ")+QString::number(m_curNet)+"-"+currentNodeStr);
    m_curId = currentNodeStr.toInt();

    //地址
    ui->lb_address->clear();
    QString nodeAdd = m_db.getNodeAddress(m_curNet,m_curId);
    ui->lb_address->setText(nodeAdd);
    //节点类型
    ui->lb_type->setText(setModType(m_curNet,m_curId));
    //节点状态
    setModSts(m_curNet,m_curId);


}

void DevInfo::slotBtnPass_1()
{
    m_curId = 0;
    m_curNet = 1;
    m_curPage = 1;
    setBtnVisible();
    lcdNumberClean();
    ui->groupBox_moudel->setTitle(tr("节点 "));
    ui->groupBox_SensorInfo->setTitle(tr("节点 "));

    m_countPage = getCountPage(m_curNet);
    ui->lb_countPage->setText(QString::number(m_countPage));
    ui->lb_curPage->setText(QString::number(m_curPage));

    ui->tBtn_pass1->setStyleSheet(m_pressedStyle);
    ui->tBtn_pass2->setStyleSheet(m_normalStyle);
}

void DevInfo::slotBtnPass_2()
{
    m_curId = 0;
    m_curNet = 2;
    m_curPage = 1;
    setBtnVisible();
    lcdNumberClean();
    ui->groupBox_moudel->setTitle(tr("节点 "));
    ui->groupBox_SensorInfo->setTitle(tr("节点 "));

    m_countPage = getCountPage(m_curNet);
    ui->lb_countPage->setText(QString::number(m_countPage));
    ui->lb_curPage->setText(QString::number(m_curPage));

    ui->tBtn_pass1->setStyleSheet(m_normalStyle);
    ui->tBtn_pass2->setStyleSheet(m_pressedStyle);
}

void DevInfo::slotBtnJump()
{
    if(ui->lineEdit->text().isEmpty() == true)
    {
        QMessageBox::information(NULL,tr("提示"),tr("请输入有效数字！"),tr("关闭"));
        return;
    }
    else
    {
        int page = ui->lineEdit->text().toInt();
        if(page > m_countPage && page != 0)
        {
            ui->lineEdit->clear();
            QMessageBox::information(NULL,tr("提示"),tr("页数超过最大总数，请重新输入！"),tr("关闭"));
            return;
        }
        else
        {
            m_curPage = page;
            setBtnVisible();
            ui->lb_curPage->setText(QString::number(m_curPage));
        }
    }
}

void DevInfo::slotBtnLastPage()
{
    if(m_curPage <= 1)
    {
        QMessageBox::information(NULL,tr("提示"),tr("已经是第一页！"),tr("关闭"));
        return;
    }
    else
    {
        m_curPage--;
        setBtnVisible();
        ui->lb_curPage->setText(QString::number(m_curPage));
    }
}

void DevInfo::slotBtnNestPage()
{
    if(m_curPage == m_countPage)
    {
        QMessageBox::information(NULL,tr("提示"),tr("已经是最后一页！"),tr("关闭"));
        return;
    }
    else
    {
        m_curPage++;
        setBtnVisible();
        ui->lb_curPage->setText(QString::number(m_curPage));
    }
}

void DevInfo::slotBtnErrorUp()
{
    if(m_errorIndex <= 0)
    {
        m_errorIndex = 0;
    }
    else
    {
        m_errorIndex--;
    }
    emit sigSetErrorValue(m_errorIndex);
}

void DevInfo::slotBtnErrorDown()
{
    m_errorIndex++;
    QString sql = "select count(*) from TEMP WHERE  type != 5;";
    int count = m_db.getRowCount(sql);
    if(count == m_errorIndex)
    {
        m_errorIndex = count - 1;
    }
    emit sigSetErrorValue(m_errorIndex);
}

void DevInfo::slotBtnAlarmUp()
{
    if(m_alarmIndex <= 0)
    {
        m_alarmIndex = 0;
    }
    else
    {
        m_alarmIndex--;
    }
    emit sigSetAlarmValue(m_alarmIndex);
}

void DevInfo::slotBtnAlarmDown()
{
    m_alarmIndex++;
    QString sql = "select count(*) FROM TEMP WHERE type = 5;";
    int count = m_db.getRowCount(sql);
    if(count == m_alarmIndex)
    {
        m_alarmIndex = count - 1;
    }
    emit sigSetAlarmValue(m_alarmIndex);
}

void DevInfo::slotResetShow()
{
    int ret = QMessageBox::information(NULL,tr("操作提示"),tr("点击确定对所有探测器进行复位"),tr("确定"),tr("取消"));
    if(ret == 0)
    {
        //声音复位
        slotSound();
        //数据库删除
        m_db.delData("delete from TEMP_ERROR;");//删除临时记录
        m_db.delData("delete from TEMP_POWER;");//删除临时记录
        for(int net = 1;net <=2;net++)
        {
            for(int id = 1;id<= 128;id++)
            {
                g_mod[net][id].dropFlag           = false;
                g_mod[net][id].normalFlag         = true;
                g_mod[net][id].phaseLossFlag      = false;
                g_mod[net][id].overCurrentFlag    = false;
                g_mod[net][id].overVoltageFlag    = false;
                g_mod[net][id].underVoltageFlag   = false;
                g_mod[net][id].interruptionFlag   = false;
                g_mod[net][id].insertDrop         = false;
                g_mod[net][id].insertphaseLoss    = false;
                g_mod[net][id].insertOverCurrent  = false;
                g_mod[net][id].insertOverVoltage  = false;
                g_mod[net][id].insertUnderVoltage = false;
                g_mod[net][id].insertInterruption = false;
            }
        }
    }

}

void DevInfo::slotBtnSound()
{
    if(m_soundFlag == TRUE)
    {
        m_soundFlag = FALSE;
        emit sigSoundType(S_NORMAL);
        ui->tBtn_voice->setText(tr("静音"));
        ui->tBtn_voice->setIcon(QIcon(":/Image/mute.png"));
    }
    else
    {
        m_soundFlag = TRUE;
        ui->tBtn_voice->setText(tr("蜂鸣"));
        ui->tBtn_voice->setIcon(QIcon(":/Image/speaker.png"));
    }
}

void DevInfo::slotSound()
{
    m_soundFlag = TRUE;
    ui->tBtn_voice->setText(tr("蜂鸣"));
    ui->tBtn_voice->setIcon(QIcon(":/Image/speaker.png"));
}



void DevInfo::slotLoginStatus(int type)
{
    if(type == MySqlite::USER || type == MySqlite::ADMIN)
    {
        ui->tBtn_reset->setEnabled(true);
        ui->tBtn_voice->setEnabled(true);
    }
    else if(type == MySqlite::QUIT)
    {
        ui->tBtn_reset->setEnabled(false);
        ui->tBtn_voice->setEnabled(false);
    }
}

void DevInfo::slotPowerSts(bool sts)
{
    //节点和电源故障
    if(errorNum > 0 || powerNum > 0 || sts == false)
    {
        if(m_soundFlag == true)
        {
            emit sigSoundType(S_ERROR);
        }
    }
    else
    {
        emit sigSoundType(S_NORMAL);
        emit sigErrorLed(false);
    }
    //    qDebug()<<"errorNum = "<<errorNum;
    //    qDebug()<<"powerNum = "<<powerNum;
    //    qDebug()<<"sts      = "<<sts;

}

void DevInfo::slotDevInfoShow()
{
    this->show();
}

void DevInfo::slotBtnBack()
{
    this->hide();
    emit sigMainPageShow();
}


