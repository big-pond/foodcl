#include "querypwdg.h"
#include "ui_querypwdg.h"
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include "def.h"
#include "proddlg.h"
#include "prodtree.h"

QueryPWdg::QueryPWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryPWdg)
{
    ui->setupUi(this);
    model = new QSqlQueryModel(this);
    ui->tableView->setModel(model);
    connect(ui->tbExecQuery, SIGNAL(clicked(bool)), SLOT(execQuery()));
    readSettings();
}

QueryPWdg::~QueryPWdg()
{
    writeSettings();
    delete ui;
}

void QueryPWdg::showEvent(QShowEvent *event)
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

void QueryPWdg::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);    // переведём окно заново
    }
    QWidget::changeEvent(event);
}

void QueryPWdg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("QueryPWdg");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void QueryPWdg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);

    settings.beginGroup("QueryPWdg");
    bool geom = settings.contains("geometry");
    if (geom)
        restoreGeometry(settings.value("geometry").toByteArray());
    else
        setGeometry(100,100,490, 300);
    settings.endGroup();
}

void QueryPWdg::execQuery()
{
    ProdDlg* dlg = new ProdDlg(this);
    dlg->getProdTree()->readData();
    if(dlg->exec()==QDialog::Accepted)
    {
        int id = dlg->getProductId();
        QSqlQuery qu(QString("SELECT name, price1kg FROM products WHERE id=%1").arg(id),QSqlDatabase::database(Def::CONN_MAIN));
        QString name;
        double price = 0;
        if(qu.next())
        {
            name = qu.value(0).toString();
            price = qu.value(1).toDouble();
        }
        qu.exec(formQueryText(id));
        model->setQuery(qu);
        settingModelAndTableView(name, price);
    }
    delete dlg;
}

QString QueryPWdg::formQueryText(int id)
{
    return QString("SELECT days.id, days.mdate AS mdate, products.name, "
              "(SELECT sum(meal.weight) FROM meal WHERE meal.days_id=days.id AND meal.product_id=products.id) AS mweight "
              "FROM days, products WHERE products.id = %1 AND mweight>0  ORDER BY days.mdate").arg(id);

}

void QueryPWdg::settingModelAndTableView(const QString& name, double price)
{
    model->setHeaderData(0, Qt::Horizontal, tr("Code"));
    model->setHeaderData(1, Qt::Horizontal, tr("Date"));
    model->setHeaderData(2, Qt::Horizontal, tr("Product (dish) name"));
    model->setHeaderData(3, Qt::Horizontal, tr("Weight, g"));

    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnHidden(2,true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    double sum = 0;

    for(int i=0; i<model->rowCount(); i++)
    {
        sum += model->data(model->index(i,3)).toDouble();
    }
    double cost = sum*price/1000.0;
    ui->lbSum->setText(tr("<b>%1</b> Total weight: %2 г  cost: €%3").arg(name).arg(sum).arg(cost,0,'f',2));
}

