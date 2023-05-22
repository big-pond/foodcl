#include "configdlg.h"
#include "ui_configdlg.h"
#include <QFileDialog>
#include <QTranslator>
#include "def.h"
#include "spinboxdelegate.h"
//#include <QDebug>

ConfigDlg::ConfigDlg(QTranslator *my_tr, QTranslator *qt_tr, QWidget *parent) : QDialog(parent),  ui(new Ui::ConfigDlg)
{
    ui->setupUi(this);
    translator = my_tr;
    qt_translator = qt_tr;
    for(int i=0; i<Def::languageCount(); i++)
        ui->cbLanguage->addItem(Def::languageName(i),Def::languageSuffix(i));
    connect(ui->tbSelDataDir, SIGNAL(clicked(bool)),SLOT(selectDataDir()));
    connect(ui->tbSelQueryDir, SIGNAL(clicked(bool)),SLOT(selectQueryDir()));
    connect(ui->cbLanguage, SIGNAL(currentIndexChanged(int)), SLOT(changeLanguage(int)));
    connect(ui->pbSetDefaultRatio, SIGNAL(clicked(bool)), SLOT(setDefaultPFCRatio()));
    ui->leDataDir->setText(Def::datadir);
    ui->leQueryDir->setText(Def::querydir);
    ui->cbLanguage->setCurrentIndex(Def::language_id);
    ui->chShowBtd->setChecked(Def::showbuttonstabdays);

    ui->label_2->setVisible(false);
    ui->leQueryDir->setVisible(false);
    ui->tbSelQueryDir->setVisible(false);
    ui->twRatio->setItemDelegate(new SpinBoxDelegate(this));
    for(int i=0; i<ui->twRatio->rowCount(); i++)
        for(int j=0; j<ui->twRatio->columnCount(); j++)
           ui->twRatio->item(i, j)->setText(QString::number(Def::pfc_ratio[i][j],'f',0));
    ui->sbVariation->setValue(Def::pfc_variation);
//    ui->tabWidget->removeTab(1);
}

ConfigDlg::~ConfigDlg()
{
    delete ui;
}

void ConfigDlg::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        //Директории при изменении языка не меняются. Запомним их содержимое.
        QString dd = ui->leDataDir->text();
        QString qd = ui->leQueryDir->text();

        ui->retranslateUi(this);

        //Директории при изменении языка не меняются. Восстановим их содержимое.
        ui->leDataDir->setText(dd);
        ui->leQueryDir->setText(qd);
    }
}

void ConfigDlg::accept()
{
    Def::datadir = ui->leDataDir->text();
    Def::querydir = ui->leQueryDir->text();
//    Def::numlanguage = ui->cbLanguage->currentIndex(); //Запоминаются в changeLanguage(int )
    Def::showbuttonstabdays = ui->chShowBtd->isChecked();
    for(int i=0; i<ui->twRatio->rowCount(); i++)
        for(int j=0; j<ui->twRatio->columnCount(); j++)
            Def::pfc_ratio[i][j] = ui->twRatio->item(i, j)->text().toDouble();
    Def::pfc_variation = ui->sbVariation->value();
    QDialog::accept();
}

void ConfigDlg::selectDataDir()
{
    QString dirname = QFileDialog::getExistingDirectory(this, tr("Select the data directory"),  // Выберите каталог данных
                                                        ui->leDataDir->text());
    if(!dirname.isEmpty())
    {
        ui->leDataDir->setText(dirname);
    }
}

void ConfigDlg::selectQueryDir()
{
    QString dirname = QFileDialog::getExistingDirectory(this, tr("Select the query directory"), // Выберите каталог запросов
                                                        ui->leQueryDir->text());
    if(!dirname.isEmpty())
    {
        ui->leQueryDir->setText(dirname);
    }
}

void ConfigDlg::changeLanguage(int )
{
    translator->load(Def::APP_NAME+ "_" + ui->cbLanguage->currentData().toString(), ".");
    qApp->installTranslator(translator);
    qt_translator->load("qtbase_" + Def::languageSuffix(Def::language_id), ".");
    qApp->installTranslator(qt_translator);
    Def::language_id = ui->cbLanguage->currentIndex();
}

void ConfigDlg::setDefaultPFCRatio()
{
    Def::setDefaultPFCRatio();
    for(int i=0; i<ui->twRatio->rowCount(); i++)
        for(int j=0; j<ui->twRatio->columnCount(); j++)
           ui->twRatio->item(i, j)->setText(QString::number(Def::pfc_ratio[i][j],'f',0));
    ui->sbVariation->setValue(Def::default_pfc_variation);
}
