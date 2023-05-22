#include "prodeddlg.h"
#include "ui_prodeddlg.h"
#include <QSettings>

ProdEdDlg::ProdEdDlg(QWidget *parent) : QDialog(parent),  ui(new Ui::ProdEdDlg)
{
    ui->setupUi(this);
    readSettings();
}

ProdEdDlg::~ProdEdDlg()
{
    writeSettings();
    delete ui;
}

void ProdEdDlg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()),QSettings::IniFormat);
    settings.beginGroup("ProdEdDlg");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void ProdEdDlg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()),QSettings::IniFormat);
    settings.beginGroup("ProdEdDlg");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

void ProdEdDlg::clear()
{
    ui->leName->clear();
    ui->sbCalories->clear();
    ui->sbCalories->clear();
    ui->sbCalories->clear();
    ui->sbCalories->clear();
    ui->sbCalories->clear();
}

QString ProdEdDlg::getName()
{
    return ui->leName->text();
}

double ProdEdDlg::getCalory()
{
    return ui->sbCalories->value();
}

double ProdEdDlg::getPrace1kg()
{
    return ui->sbPrice->value();
}

double ProdEdDlg::getProt()
{
    return ui->sbProt->value();
}

double ProdEdDlg::getFat()
{
    return ui->sbFat->value();
}

double ProdEdDlg::getCarb()
{
    return ui->sbCarbo->value();
}

void ProdEdDlg::setData(const QString& name, const double& calory,
                        const double& prot,  const double& fat,
                        const double& carb, const double& price)
{
    ui->leName->setText(name);
    ui->sbCalories->setValue(calory);
    ui->sbProt->setValue(prot);
    ui->sbFat->setValue(fat);
    ui->sbCarbo->setValue(carb);
    ui->sbPrice->setValue(price);
}
