#include "querypfcwdg.h"
#include "ui_querypfcwdg.h"
#include <QMessageBox>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include "def.h"

//#include <QDebug>

QueryPFCWdg::QueryPFCWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryPFCWdg)
{
    ui->setupUi(this);
    model = new QSqlQueryModel(this);
    ui->tableView->setModel(model);
    connect(ui->tbExecQuery, SIGNAL(clicked(bool)), SLOT(execQuery()));
    readSettings();
}

QueryPFCWdg::~QueryPFCWdg()
{
    writeSettings();
    delete ui;
}

void QueryPFCWdg::showEvent(QShowEvent *event)
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

void QueryPFCWdg::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);    // переведём окно заново
    }
    QWidget::changeEvent(event);
}

void QueryPFCWdg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("QueryPFCWdg");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void QueryPFCWdg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);

    settings.beginGroup("QueryPFCWdg");
    bool geom = settings.contains("geometry");
    if (geom)
        restoreGeometry(settings.value("geometry").toByteArray());
    else
        setGeometry(100,100,490, 300);
    settings.endGroup();

}

void QueryPFCWdg::execQuery()
{
    if(ui->deDate1->date()>ui->deDate2->date())
    {
        QMessageBox::information(this, "", tr("Dates selected incorrectly. %1 greater than %2.")//Не правильно выбраны даты.
                                 .arg(ui->deDate1->date().toString(Qt::ISODate))
                                 .arg(ui->deDate2->date().toString(Qt::ISODate)));
        return;
    }

    QString term = QString("days.mdate>='%1' AND days.mdate<='%2'")
            .arg(ui->deDate1->date().toString(Qt::ISODate))
            .arg(ui->deDate2->date().toString(Qt::ISODate));
    QSqlQuery qu(QSqlDatabase::database(Def::CONN_MAIN));
    double avcl = 0, frpr = 0, frft = 0, frcr = 0;
    qu.exec(QString("SELECT sum(meal.weight*products.calory/100.0)/(SELECT count() FROM days WHERE %1) "
                    "FROM meal, products, days WHERE  meal.product_id=products.id AND meal.days_id=days.id AND %1").arg(term));
    if(qu.next()) avcl =  qu.value(0).toDouble();

    qu.exec(QString("SELECT round(sum(meal.weight*products.protein)/sum(meal.weight*products.protein+meal.weight*products.fat+meal.weight*products.carbohydrate),3) "
                    "FROM meal, products, days WHERE  meal.product_id=products.id AND meal.days_id=days.id AND %1").arg(term));
    if(qu.next()) frpr =  qu.value(0).toDouble();
    qu.exec(QString("SELECT round(sum(meal.weight*products.fat)/sum(meal.weight*products.protein+meal.weight*products.fat+meal.weight*products.carbohydrate),3) "
                    "FROM meal, products, days WHERE  meal.product_id=products.id AND meal.days_id=days.id AND %1").arg(term));
    if(qu.next()) frft =  qu.value(0).toDouble();
    qu.exec(QString("SELECT round(sum(meal.weight*products.carbohydrate)/sum(meal.weight*products.protein+meal.weight*products.fat+meal.weight*products.carbohydrate),3) "
                    "FROM meal, products, days WHERE  meal.product_id=products.id AND meal.days_id=days.id AND %1").arg(term));
    if(qu.next()) frcr =  qu.value(0).toDouble();

    qu.exec(formQueryText(term));
    model->setQuery(qu);
    settingModelAndTableView(avcl,frpr,frft,frcr);
}

QString QueryPFCWdg::formQueryText(const QString& term)
{
    QString querytext;
    querytext = QString("SELECT days.id, days.mdate, "
                        "(SELECT sum(meal.weight*products.calory/100) FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id) AS ccal, "
                        "(SELECT round(sum(meal.weight*products.protein)/sum(meal.weight*products.protein+meal.weight*products.fat+meal.weight*products.carbohydrate),2) "
                            "FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id) AS pprot, "
                        "(SELECT round(sum(meal.weight*products.fat)/sum(meal.weight*products.protein+meal.weight*products.fat+meal.weight*products.carbohydrate),2) "
                            "FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id) AS pfats, "
                        "(SELECT round(sum(meal.weight*products.carbohydrate)/sum(meal.weight*products.protein+meal.weight*products.fat+meal.weight*products.carbohydrate),2) "
                            "FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id) AS pcarb "
                        "FROM days WHERE %1 ORDER BY days.mdate").arg(term);
    return querytext;
}

void QueryPFCWdg::settingModelAndTableView(double avcl, double frpr, double frft, double frcr)
{
    model->setHeaderData(0, Qt::Horizontal, tr("Code"));
    model->setHeaderData(1, Qt::Horizontal, tr("Date"));
    model->setHeaderData(2, Qt::Horizontal, tr("Calorie\nintake,\nkcal"));
    model->setHeaderData(3, Qt::Horizontal, tr("Protein,\nfraction"));
    model->setHeaderData(4, Qt::Horizontal, tr("Fat,\nfraction"));
    model->setHeaderData(5, Qt::Horizontal, tr("Carbohydrate,\nfraction"));

    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->lbSum->setText(tr("Average calorie intake: %1 kcal.  Average fraction: protein %2;  fat %3;  carbohydrate %4")
                       .arg(avcl,0,'f',1)
                       .arg(frpr,0,'f',2)
                       .arg(frft,0,'f',2)
                       .arg(frcr,0,'f',2));

}

