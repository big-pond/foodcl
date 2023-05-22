#include "prodwdgdlg.h"
#include "ui_prodwdgdlg.h"
#include <QPushButton>
#include <QSettings>
#include "prodwdg.h"
#include "prodtree.h"

ProdwdgDlg::ProdwdgDlg(QWidget *parent) : QDialog(parent), ui(new Ui::ProdwdgDlg)
{
    ui->setupUi(this);
    QGridLayout* gl = ui->gridLayout;
    prodwdg = new ProdWdg;
    gl->addWidget(prodwdg,0,0, 1,1);
//    ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(false);
    readSettings();
}

ProdwdgDlg::~ProdwdgDlg()
{
    writeSettings();
    delete ui;
}

void ProdwdgDlg::readData()
{
    prodwdg->getProdTree()->readData();
}

void ProdwdgDlg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()),QSettings::IniFormat);
    settings.beginGroup("ProdwdgDlg");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void ProdwdgDlg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()),QSettings::IniFormat);
    settings.beginGroup("ProdwdgDlg");
    bool geom = settings.contains("geometry");
    if (geom)
        restoreGeometry(settings.value("geometry").toByteArray());
    else
        setGeometry(100,25,444, 575);
    settings.endGroup();
}

