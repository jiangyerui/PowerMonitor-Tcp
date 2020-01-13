#include "record.h"
#include "ui_record.h"


Record::Record(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Record)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0,0,800,480);
    initVar();
    initConnect();
    initTableWidget(ui->tableWidget);
}

Record::~Record()
{
    delete ui;
}

void Record::initVar()
{

    m_currentPage = 1;
    m_model = new QSqlQueryModel();

    m_myPrint = MyPrint::getPrint();
    m_myPrint->initCom();

    //隐藏所有记录，通道，
    ui->checkBox_id->hide();
    ui->checkBox_net->hide();
    ui->lineEdit_address->hide();
    ui->lineEdit_pass->hide();



    //正则表达式表示只能输入大于0的正整数 只能是数字^[0-9]*$
    QRegExp regExp("^[1-9][0-9]*$");
    ui->lineEdit_pageNum->setValidator(new QRegExpValidator(regExp, this));
    //地址
    QRegExp regExpNumId("^([1-9][0-9]{1,2})|(101[0-9])|(102[0-4])|0$");//1-1024
    ui->lineEdit_address->setValidator(new QRegExpValidator(regExpNumId,this));
    //通道
    QRegExp regExpNumNet("^[1-2]$");//1-2
    ui->lineEdit_pass->setValidator(new QRegExpValidator(regExpNumNet,this));

    //上下翻页
    ui->tBtn_downPage->setEnabled(false);
    ui->tBtn_upPage->setEnabled(false);
    //net id 编辑框
    ui->lineEdit_address->setEnabled(false);
    ui->lineEdit_pass->setEnabled(false);
    //时间选择
    ui->dTEdit_end->setDateTime(QDateTime::currentDateTime());
    ui->dTEdit_start->setDateTime(QDateTime(QDate(2000,1,1),QTime(0,0,1)));
}

void Record::initConnect()
{

    connect(ui->tBtn_quit,SIGNAL(clicked(bool)),this,SLOT(hide()));
    connect(ui->tBtn_jump,SIGNAL(clicked(bool)),this,SLOT(slotBtnJump()));
    connect(ui->tBtn_del,SIGNAL(clicked(bool)),this,SLOT(slotBtnDelData()));
    connect(ui->tBtn_print,SIGNAL(clicked(bool)),this,SLOT(slotBtnPrint()));
    connect(ui->tBtn_check,SIGNAL(clicked(bool)),this,SLOT(slotBtnSearch()));
    connect(ui->tBtn_upPage,SIGNAL(clicked(bool)),this,SLOT(slotBtnUpPage()));
    connect(ui->tBtn_downPage,SIGNAL(clicked(bool)),this,SLOT(slotBtnDownPage()));
    connect(ui->checkBox_id,SIGNAL(clicked(bool)),this,SLOT(slotSetIdEnable(bool)));
    connect(ui->checkBox_net,SIGNAL(clicked(bool)),this,SLOT(slotSetNetEnable(bool)));

}

void Record::initTableWidget(QTableWidget *tableWidget)
{

    QStringList headList;
    tableWidget->setColumnCount(5);
    headList<<tr("通道")<<tr("地址")<<tr("状态")<<tr("时间")<<tr("详细地址");
    tableWidget->setHorizontalHeaderLabels(headList);
    tableWidget->horizontalHeader()->setFixedHeight(27);

    QString styleStr = "QHeaderView::section{ background-color:rgb(255,255,255);color: black;border: 1px solid #6c6c6c;}";
    tableWidget->horizontalHeader()->setStyleSheet(styleStr);
    tableWidget->verticalHeader()->setStyleSheet(styleStr);

    tableWidget->horizontalHeader()->setEnabled(false);
    tableWidget->horizontalHeader()->setVisible(true);//设置表头显示
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setHighlightSections(false);

    tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    tableWidget->setSelectionBehavior (QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    tableWidget->setSelectionMode (QAbstractItemView::SingleSelection); //设置选择模式，选择单行

    tableWidget->setColumnWidth(0,80);
    tableWidget->setColumnWidth(1,80);
    tableWidget->setColumnWidth(2,150);
    tableWidget->setColumnWidth(3,190);
    tableWidget->setColumnWidth(4,190);

}



void Record::setBtnEnable(bool enabel, int current, int maxPage)
{
    ui->tBtn_downPage->setEnabled(enabel);
    ui->tBtn_upPage->setEnabled(enabel);
    ui->tBtn_jump->setEnabled(enabel);
    ui->tBtn_print->setEnabled(enabel);
    ui->tBtn_del->setEnabled(enabel);
    ui->lineEdit_pageNum->setEnabled(enabel);
    ui->lb_count->setText(QString::number(maxPage));
    ui->lb_current->setText(QString::number(current));
}

void Record::delAllData()
{
    QString sql="delete from RECORD;";
    if(m_db.delData(sql))
    {
        ui->lb_count->setText("0");
        ui->lb_current->setText("0");
        ui->tableWidget->clearContents();
    }
    else
    {
        QMessageBox::information(NULL,tr("删除数据"),tr("删除数据失败"),tr("确定"));
    }
}

void Record::delRowData(QString netStr, QString idStr, QString timeStr)
{
    QString sql="delete from RECORD where net = "+netStr+" and id = "+idStr+" and time = "+timeStr+";";

    if(m_db.delData(sql))
    {
        if(ui->tableWidget->rowCount() == 1 && m_maxPage != 1)
        {
            m_currentPage--;
        }
    }
    else
    {
        QMessageBox::information(NULL,tr("删除数据"),tr("删除数据失败"),tr("确定"));
    }
}

QString Record::setSelectSql(int item, bool net, bool id, QString &startTimer, QString &endTime)
{
    QString netStr;
    QString idStr;
    QString sqlStr = "select net,id,status,time,address from RECORD where TIME >= "+startTimer+" and TIME <= "+endTime;

    if(item == ALLDATA)
    {
        if(net == true && id == true)//获取指定的探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and net = "+ netStr +" and id = "+idStr;
        }
        else if(net == true && id == false)//获取指定网络探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            sqlStr += " and net = "+ netStr;
        }
        else if(net == false && id == true)//获取指定节点探测器报警信息
        {
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and id = "+idStr;
        }
    }
    else if(item == ALARMDATA)
    {
        if(net == true && id == true)//获取指定的探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and net = "+ netStr +" and id = "+idStr+" and status = 5 ";
        }
        else if(net == false && id == false)//获取所有探测器报警信息
        {

            sqlStr += " and status = 5 ";
        }
        else if(net == true && id == false)//获取指定网络探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            sqlStr += " and net = "+ netStr +" and status = 5 ";
        }
        else if(net == false && id == true)//获取指定节点探测器报警信息
        {
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and id = "+idStr+" and status = 5 ";
        }
    }
    else if(item == ERRORDATA)
    {
        if(net == true && id == true)//获取指定的探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and net = "+ netStr +" and id = "+idStr+" and status between 2 and 4 ";
        }
        else if(net == false && id == false)//获取所有探测器报警信息
        {

            sqlStr += " and status between 2 and 4 ";
        }
        else if(net == true && id == false)//获取指定网络探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            sqlStr += " and net = "+ netStr +" and status between 2 and 4 ";
        }
        else if(net == false && id == true)//获取指定节点探测器报警信息
        {
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and id = "+idStr+" and status between 2 and 4 ";
        }
    }
    return sqlStr;

}

void Record::slotBtnJump()
{
    int num = ui->lineEdit_pageNum->text().toInt();
    if(num > m_maxPage)
    {
        QMessageBox::question( NULL, tr("提示信息"), tr("不能超过最大页数!"), tr("确定"));
        ui->lineEdit_pageNum->setText(QString::number(m_maxPage));
    }
    else
    {
        m_currentPage = num;
        slotBtnSearch();
    }
}

void Record::slotBtnPrint()
{
//    for(int i = 2;i<129;i++)
//    {
//        m_db.insertNode(2,i,"qweqweqwe");
//    }

    int index = ui->QCom_del->currentIndex();
    if(index == 0)
    {

        //判断有没有选中一行
        int row = ui->tableWidget->currentRow();
        if(row >= 0)
        {

            setPrint(ui->tableWidget,m_myPrint,row,ui->tableWidget->columnCount());
        }

    }
    else if(index == 1)
    {

        int rowNum = ui->tableWidget->rowCount();
        if(rowNum > 0)
        {

            for(int row = 0;row < rowNum;row++)
            {
                setPrint(ui->tableWidget,m_myPrint,row,ui->tableWidget->columnCount());
            }
        }
    }
}

int Record::getPageCount(QString sql)
{
    int countPage = 0;
    int recordNum = m_db.getRowCount(sql);//返回相应查询的条数
    if(0 >= recordNum)  //没有相应的记录返
    {
        return countPage;
    }
    else
    {
        if(recordNum % 10)//非整数页
        {
            countPage = recordNum/10;
            countPage++;
        }
        else//整数页
        {
            countPage = recordNum/10;
        }
    }
    return countPage;
}

void Record::showData(QTableWidget *tableWidget, QString sql, int currentPage)
{
    //设置每次返回的条数10条

    int baseNum = (currentPage - 1) * 10;
    sql += " ORDER BY time DESC limit 10 offset "+QString::number(baseNum)+";";
    int countNum = m_db.getRowCount(sql,m_model);//返回数据的总行数

#ifdef DEBUG
    qDebug()<<"sql      = "<<sql;
    qDebug()<<"countNum = "<<countNum;
#endif
    tableWidget->clearContents();
    tableWidget->setRowCount(countNum);//设置行数
    //qDeleteAll(m_itemList);
    m_itemList.clear();
    QTableWidgetItem *item;
    QSqlRecord record;

    for(int row = 0; row < countNum; row++)
    {
        record = m_model->record(row);

        tableWidget->setRowHeight(row,29);
        for(int column = 0; column < 5; column++)
        {
            item = new QTableWidgetItem;
            m_itemList.append(item);

            item->setFont(QFont("文泉驿等宽微米黑",15));
            item->setTextAlignment(Qt::AlignCenter);
            item->setTextColor(QColor(0,0,0));

            if(column == 0)
            {
                item->setText(record.value(column).toString());
                tableWidget->setItem( row, column, item);
            }
            else if(column == 1)
            {
                item->setText(record.value(column).toString());
                tableWidget->setItem( row, column, item);
            }
            else if(column == 2)
            {
                if(POWERBREAK == record.value(column).toUInt())
                {
                    item->setText(tr("主电故障"));
                    tableWidget->setItem( row, column, item);
                }
                else if(BPOWERBREAK == record.value(column))
                {
                    item->setText(tr("备电故障"));
                    tableWidget->setItem( row, column, item);
                }
                else if(BPOWERSHORT == record.value(column))
                {
                    item->setText(tr("备电短路"));
                    tableWidget->setItem( row, column, item);
                }
                else if(OVERCURRENT == record.value(column))
                {
                    item->setText(tr("过流报警"));
                    tableWidget->setItem( row, column, item);
                }
                else if(PHASELOSS == record.value(column))
                {
                    item->setText(tr("缺相故障"));
                    tableWidget->setItem( row, column, item);
                }
                else if(OVERVOLTAGE == record.value(column))
                {
                    item->setText(tr("过压故障"));
                    tableWidget->setItem( row, column, item);
                }
                else if(UNDERVOLTAGE == record.value(column))
                {
                    item->setText(tr("欠压故障"));
                    tableWidget->setItem( row, column, item);
                }
                else if(INTERRUPTION == record.value(column))
                {
                    item->setText(tr("供电中断"));
                    tableWidget->setItem( row, column, item);
                }
                else if(CANERROR == record.value(column))
                {
                    item->setText(tr("通讯故障"));
                    tableWidget->setItem( row, column, item);
                }
            }
            else if(column == 3)
            {
                int dt = record.value(column).toUInt();
                QDateTime datetime; datetime.setTime_t(dt);
                QString dtstr=datetime.toString("yyyy-MM-dd/hh:mm:ss");
                item->setText(dtstr);
                tableWidget->setItem( row, column, item);
            }
            else if(column == 4)
            {
                item->setText(record.value(column).toString());
                tableWidget->setItem( row, column, item);
            }

        }
    }
}

void Record::setPrint(QTableWidget *tableWidget, MyPrint *myPrint, int row, int columnCount)
{
    QList <QString> printList;

    for(int column = 0;column < columnCount;column++)
    {
        QString str = tableWidget->item(row,column)->text();
        printList.insert(column,str);
    }

    QString net    = printList.at(0);
    QString id     = printList.at(1);
    QString type   = printList.at(2);
    QString status = printList.at(2);
    QString value  = printList.at(4);
    QString time   = printList.at(3);//时间
    QString add    = tr("**北京联创广汇**");

    /*
    QString net    = printList.at(0);
    QString id     = printList.at(1);
    QString type   = printList.at(2);
    QString status = printList.at(3);
    QString value  = printList.at(4);
    QString time   = printList.at(5);
    QString add    = printList.at(6);
*/

    myPrint->printConnect(net, id, type,status,value, time,add);

    printList.clear();




}

void Record::slotBtnSearch()
{
    int item = ui->cbBox_type->currentIndex();
    bool net = ui->checkBox_net->isChecked();
    bool  id = ui->checkBox_id->isChecked();

    uint starTime = ui->dTEdit_start->dateTime().toTime_t();
    uint endTime  = ui->dTEdit_end->dateTime().toTime_t();

    if(starTime == endTime)
    {
        QMessageBox::question( NULL, tr("提示信息"), tr("请选择时间范围!"), tr("确定"));
    }
    else if(starTime > endTime)
    {
        QMessageBox::question( NULL, tr("提示信息"), tr("选择时间范围错误!"), tr("确定"));
    }
    else
    {
        QString countSql;
        QString starTimeStr = QString::number(starTime);
        QString endTimeStr  = QString::number(endTime);
        QString sqlStr = setSelectSql(item,net,id,starTimeStr,endTimeStr);
        QString str ="WHERE";
        countSql = "select count(*) from RECORD WHERE" + sqlStr.mid(sqlStr.indexOf(str,0,Qt::CaseInsensitive)+5,sqlStr.size() - sqlStr.indexOf(str,0))+";";
        m_maxPage = getPageCount(countSql);

#ifdef  DEBUG
        qDebug()<<"net           = "<<net;
        qDebug()<<"id            = "<<id;
        qDebug()<<"countSql      = "<<countSql;
        qDebug()<<"sqlStr        = "<<sqlStr;
        qDebug()<<"m_maxPage     = "<<m_maxPage;
        qDebug()<<"m_currentPage = "<<m_currentPage;
#endif

        if(m_currentPage > m_maxPage )
        {
            m_currentPage = 1;
        }

        if(m_maxPage > 0)
        {
            showData(ui->tableWidget,sqlStr,m_currentPage);
            setBtnEnable(true,m_currentPage,m_maxPage);
        }
        else if(m_maxPage == 0)
        {
            ui->tableWidget->clearContents();
            ui->tableWidget->setRowCount(0);//设置行数
            setBtnEnable(false,0,0);
            ui->tBtn_print->setEnabled(true);
        }
    }
}

void Record::slotBtnUpPage()
{
    if(m_currentPage > 1)
    {
        m_currentPage--;
        slotBtnSearch();
        ui->tBtn_upPage->setEnabled(true);
    }
    else if(m_currentPage == 1 || m_maxPage == 0)
    {
        ui->tBtn_upPage->setEnabled(false);
        QMessageBox::information(NULL,tr("提示"),tr("已经是第一页！"),tr("确定"));
    }
    ui->tBtn_downPage->setEnabled(true);
}

void Record::slotBtnDelData()
{
    int index = ui->QCom_del->currentIndex();
    if(index == 0)
    {
        int row = ui->tableWidget->currentRow();
        if(row < 0)
        {
            return;
        }
        //先删除数据
        QString netStr  = ui->tableWidget->item(row,0)->text();
        QString idStr   = ui->tableWidget->item(row,1)->text();
        QString timeStr = ui->tableWidget->item(row,3)->text();
        uint second     = QDateTime::fromString(timeStr,"yyyy-MM-dd/hh:mm:ss").toTime_t();
        delRowData(netStr,idStr,QString::number(second));
        //查询数据
        ui->tableWidget->clearContents();
        slotBtnSearch();
    }
    else if(index == 1)
    {
        int rowCount = ui->tableWidget->rowCount();
        QString netStr,idStr,timeStr;
        uint second ;
        for(int row = 0;row < rowCount; row++)
        {
            netStr  = ui->tableWidget->item(row,0)->text();
            idStr   = ui->tableWidget->item(row,1)->text();
            timeStr = ui->tableWidget->item(row,3)->text();
            second  = QDateTime::fromString(timeStr,"yyyy-MM-dd/hh:mm:ss").toTime_t();
            delRowData(netStr,idStr,QString::number(second));
        }
        ui->tableWidget->clearContents();
        if(m_currentPage == m_maxPage && m_maxPage != 1)
        {
            m_currentPage--;
        }
        slotBtnSearch();
    }
    else if(index == 2)
    {
        delAllData();
        slotBtnSearch();
    }
}

void Record::slotBtnDownPage()
{
    if(m_currentPage < m_maxPage)
    {
        m_currentPage++;
        slotBtnSearch();
        ui->tBtn_downPage->setEnabled(true);
    }
    else if(m_currentPage == m_maxPage || m_maxPage == 0)
    {
        ui->tBtn_downPage->setEnabled(false);
        QMessageBox::information(NULL,tr("提示"),tr("已经是最后一页！"),tr("确定"));
    }
    ui->tBtn_upPage->setEnabled(true);
}

void Record::slotSetIdEnable(bool enable)
{
    ui->lineEdit_address->setEnabled(enable);
}

void Record::slotSetNetEnable(bool enable)
{
    ui->lineEdit_pass->setEnabled(enable);
}

void Record::slotRecordShow()
{
    this->show();

//    if(m_userType == MySqlite::ADMIN)
//    {
//        ui->tBtn_del->setVisible(true);
//    }
//    else
//    {
//        ui->tBtn_del->setVisible(false);
//    }

    m_currentPage = 1;
    ui->lb_count->setText("0");
    ui->lb_current->setText("0");

    ui->lineEdit_pass->clear();
    ui->lineEdit_pass->setEnabled(false);

    ui->lineEdit_address->clear();
    ui->lineEdit_address->setEnabled(false);

    ui->lineEdit_pageNum->clear();
    ui->lineEdit_pageNum->setEnabled(false);

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    ui->checkBox_net->setCheckState(Qt::Unchecked);
    ui->checkBox_id->setCheckState(Qt::Unchecked);
    ui->dTEdit_end->setDateTime(QDateTime::currentDateTime());

    ui->tBtn_jump->setEnabled(false);
    ui->tBtn_upPage->setEnabled(false);
    ui->tBtn_downPage->setEnabled(false);

}

void Record::slotLoginStatus(int type)
{
    if(type == MySqlite::ADMIN)
    {
        ui->tBtn_del->setVisible(true);
    }
    else
    {
        ui->tBtn_del->setVisible(false);
    }
}
