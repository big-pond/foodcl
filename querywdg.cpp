#include "querywdg.h"
#include "ui_querywdg.h"
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include "def.h"
#include "groupdlg.h"

//#include <QDebug>

QueryWdg::QueryWdg(QWidget *parent) :  QWidget(parent),  ui(new Ui::QueryWdg)
{
    ui->setupUi(this);
    model = new QSqlQueryModel(this);
    ui->tableView->setModel(model);
    connect(ui->tbExecQuery, SIGNAL(clicked(bool)), SLOT(execQuery()));
    readSettings();
}

QueryWdg::~QueryWdg()
{
    writeSettings();
    delete ui;
}

void QueryWdg::showEvent(QShowEvent *event)
{
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlQuery query(QString("SELECT min(mdate), max(mdate) FROM days"),db);
    if(query.next())
    {
        ui->deDate1->setDate(query.value(0).toDate());
        ui->deDate2->setDate(query.value(1).toDate());
    }
    QWidget::showEvent(event);
}

void QueryWdg::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void QueryWdg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("QueryWdg");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void QueryWdg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);

    settings.beginGroup("QueryWdg");
    bool geom = settings.contains("geometry");
    if (geom)
        restoreGeometry(settings.value("geometry").toByteArray());
    else
        setGeometry(100,100,490, 300);
    settings.endGroup();
}

void QueryWdg::settingModelAndTableView()
{
    model->setHeaderData(0, Qt::Horizontal, tr("Code"));
    model->setHeaderData(1, Qt::Horizontal, tr("Product (dish) name"));//Продукт (готовое блюдо)
    model->setHeaderData(2, Qt::Horizontal, tr("Weight, g"));
    model->setHeaderData(3, Qt::Horizontal, tr("Price, €"));
    model->setHeaderData(4, Qt::Horizontal, tr("Cost, €"));

    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setColumnHidden(3,true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}


void QueryWdg::execQuery()
{
    int groupid = -1;
    GroupDlg* gdlg = new GroupDlg(this);
    gdlg->readData();
    gdlg->setCurrentTopLevelFirstItem();
    if(gdlg->exec()==QDialog::Accepted) //Выбирали группу
    {
        groupid = gdlg->getSelectedGroupID();
        if(groupid>0)
        {
            QSqlQuery qu(formQueryGrpText(gdlg->getListGroupID()),QSqlDatabase::database(Def::CONN_MAIN));
            model->setQuery(qu);
            execQueryGrpSum();
            settingModelAndTableView();
        }
    }
    delete gdlg;
    if(groupid==-1) //Не выбирали группу (Все продукты)
    {
        QSqlQuery qu(formQueryAllText(),QSqlDatabase::database(Def::CONN_MAIN));
        model->setQuery(qu);
        execQueryAllSum();
        settingModelAndTableView();
    }
}

QString QueryWdg::formQueryGrpText(QList<int> idlist)
{
    QString querytext;
    term1 = QString("(products.parent_id=%1").arg(idlist.at(0));
    for(int i=1; i<idlist.count(); i++)
    {
        term1.append(tr(" OR products.parent_id=%1").arg(idlist.at(i)));
    }
    term1.append(")");
    term2 = QString("AND days.mdate>='%1' AND days.mdate<='%2'")
            .arg(ui->deDate1->date().toString(Qt::ISODate))
            .arg(ui->deDate2->date().toString(Qt::ISODate));
    querytext = QString("SELECT products.id, products.name, sum(meal.weight), "
                        "products.price1kg, sum(meal.weight)*products.price1kg/1000.0 "
                        "FROM days, products, meal WHERE %1 AND  "
                        "days.id=meal.days_id AND meal.product_id=products.id %2 "
                        "GROUP BY meal.product_id ORDER BY products.name").arg(term1).arg(term2);
    return querytext;
}

QString QueryWdg::formQueryAllText()
{
    QString querytext = QString("SELECT products.id, products.name, sum(meal.weight), "
               "products.price1kg, sum(meal.weight)*products.price1kg/1000.0 "
               "FROM days, products, meal WHERE meal.days_id=days.id AND meal.product_id=products.id "
                "AND (days.mdate<='%1' AND days.mdate>='%2') "
               "GROUP BY meal.product_id ORDER BY products.name")
            .arg(ui->deDate2->date().toString(Qt::ISODate))
            .arg(ui->deDate1->date().toString(Qt::ISODate));
    return querytext;
}

void QueryWdg::execQueryAllSum()
{
    double sumw = 0, sumc = 0;
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlQuery qu(db);
    qu.exec(QString("SELECT sum(meal.weight), "
                    "sum(meal.weight*products.price1kg/1000.0) "
                    "FROM days, meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id  "
                    "AND (days.mdate<='%1'  AND days.mdate>='%2')")
            .arg(ui->deDate2->date().toString(Qt::ISODate))
            .arg(ui->deDate1->date().toString(Qt::ISODate)));

    if(qu.next())
    {
        sumw = qu.value(0).toDouble();
        sumc = qu.value(1).toDouble();
    }
    ui->lbSum->setText(tr("Total weight: %1 g     Total cost: €%2").arg(sumw).arg(sumc,0,'f',2)); //Вес съеденного: %1 г     На сумму: %2 руб
}

void QueryWdg::execQueryGrpSum()
{
    double sumw = 0, sumc = 0;
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlQuery qu(db);
    qu.exec(QString("SELECT sum(meal.weight), "
                    "sum(meal.weight*products.price1kg/1000.0) "
                    "FROM days, meal, products WHERE %1 AND "
                    "days.id=meal.days_id AND meal.product_id=products.id  %2")
            .arg(term1).arg(term2));
    if(qu.next())
    {
        sumw = qu.value(0).toDouble();
        sumc = qu.value(1).toDouble();
    }
    ui->lbSum->setText(tr("Total weight: %1 g     Total cost: €%2").arg(sumw).arg(sumc,0,'f',2));
}

