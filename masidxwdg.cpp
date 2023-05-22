#include "masidxwdg.h"
#include "ui_masidxwdg.h"
#include <QSettings>
#include <QSqlQuery>
#include "indicators.h"
#include "def.h"

MasIdxWdg::MasIdxWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MasIdxWdg)
{
    ui->setupUi(this);
    for(int i=0; i<Def::activityCount(); i++)
        ui->cbActivity->addItem(Def::activityName(i),Def::activityData(i));
    connect(ui->sbWeight, SIGNAL(valueChanged(double)), this, SLOT(calc()));
    connect(ui->sbHeight, SIGNAL(valueChanged(int)), this, SLOT(calc()));
    connect(ui->sbOld, SIGNAL(valueChanged(int)), this, SLOT(calc()));
    connect(ui->cbActivity, SIGNAL(currentIndexChanged(int)), this, SLOT(calc()));
    connect(ui->cbSex, SIGNAL(currentIndexChanged(int)), this, SLOT(calc()));
    connect(ui->pbClouse, SIGNAL(clicked()), this, SLOT(close()));
    ui->tableWidget->resizeColumnsToContents();
    readSettings();
  }

MasIdxWdg::~MasIdxWdg()
{
    writeSettings();
    delete ui;
}

void MasIdxWdg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()),QSettings::IniFormat);
    settings.beginGroup("MasIdxWdg");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void MasIdxWdg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()),QSettings::IniFormat);
    settings.beginGroup("MasIdxWdg");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

void MasIdxWdg::showEvent(QShowEvent *event)
{
    selectData();
    calc();
    QWidget::showEvent(event);
}

void MasIdxWdg::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
}
void MasIdxWdg::selectData()
{
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlQuery query("SELECT * FROM person", db);
    if(query.next())
    {
        QDate dob = QDate::fromString(query.value("dob").toString(), Qt::ISODate);
        ui->sbOld->setValue( Indicators::yearsold(dob));
        double height = query.value("height").toDouble();
        ui->sbHeight->setValue(height);
        ui->cbSex->setCurrentIndex(query.value("sex").toInt());
        ui->cbActivity->setCurrentIndex(query.value("activity").toInt());
        QSqlQuery query2("SELECT max(mdate), weight FROM days, weight WHERE days.id=weight.days_id", db);
        if(query2.next())
        {
            double weight = query2.value(1).toDouble();
            ui->sbWeight->setValue(weight);
        }
    }
}


void MasIdxWdg::calc()
{
    double h = ui->sbHeight->value();
    double w = ui->sbWeight->value();
    double old = ui->sbOld->value();
    int sex = ui->cbSex->currentIndex();
    double coefa = ui->cbActivity->currentData().toDouble();
    double im = Indicators::imb(w, h);
    double kmj = Indicators::fMaffinJeor(w,h,old,sex);
    double w1 = Indicators::wimb(18.5, h);
    double w2 = Indicators::wimb(25.0, h);
    ui->tableWidget->item(0,1)->setText(QString::number(im, 'f', 2));
    ui->tableWidget->item(0,1)->setToolTip(Indicators::noteImb(im));

    ui->tableWidget->item(1,1)->setText(QString::number(kmj, 'f', 1));
    ui->tableWidget->item(2,1)->setText(QString::number(kmj*coefa, 'f', 1));
    ui->tableWidget->item(3,1)->setText(tr("%1 - %2").arg(w1,0,'f',1).arg(w2,0,'f',1));
    ui->tableWidget->resizeColumnsToContents();
}
