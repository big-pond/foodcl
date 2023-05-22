#include "persondlg.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QSqlQuery>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>
#include "def.h"
#include "indicators.h"

//#include <QDebug>

PersonDlg::PersonDlg(QWidget *parent) :
    QDialog(parent)
{
    QGridLayout* gl = new QGridLayout;

    int row = 0;

    gl->addWidget(new QLabel(tr("Name")),row,0);
    leName = new QLineEdit;
    leName->setMinimumWidth(200);
    gl->addWidget(leName,row,1);
    row++;

    gl->addWidget(new QLabel(tr("Birthdate")),row,0);
    deDOB = new QDateEdit;
    deDOB->setMaximumWidth(90);
    gl->addWidget(deDOB,row,1);
    row++;

    gl->addWidget(new QLabel(tr("Gender")),row,0);
    cbSex = new QComboBox;
    cbSex->addItem(tr("Female"));
    cbSex->addItem(tr("Male"));
    cbSex->setMaximumWidth(90);
    gl->addWidget(cbSex,row,1);
    row++;

    gl->addWidget(new QLabel(tr("Height, cm")),row,0);
    sbHeight = new QSpinBox;
    sbHeight->setMinimum(100);
    sbHeight->setMaximum(250);
    sbHeight->setMaximumWidth(60);
    gl->addWidget(sbHeight,row,1);
    row++;

    gl->addWidget(new QLabel(tr("Activity")),row,0);
    cbActive = new QComboBox;
    for(int i=0; i<Def::activityCount(); i++)
        cbActive->addItem(Def::activityName(i),Def::activityData(i));
    gl->addWidget(cbActive,row,1,1,2);
    row++;

    gl->addWidget(new QLabel(tr("<font color=\"#0000FF\">Ratio of protein, fat, carbohydrates to achieve the goal:</font>")),row,0,1,2, Qt::AlignHCenter);
    row++;

    gl->addWidget(new QLabel(tr("Proteins, %")),row,0); //proteins, fats, carbohydrates
    sbProt = new QSpinBox;
    sbProt->setMinimum(0);
    sbProt->setMaximum(100);
    sbProt->setMaximumWidth(60);
    gl->addWidget(sbProt,row,1);
    row++;

    gl->addWidget(new QLabel(tr("Fats, %")),row,0);
    sbFat = new QSpinBox;
    sbFat->setMinimum(0);
    sbFat->setMaximum(100);
    sbFat->setMaximumWidth(60);
    gl->addWidget(sbFat,row,1);
    row++;

    gl->addWidget(new QLabel(tr("Carbohydrates, %")),row,0);
    sbCarb = new QSpinBox;
    sbCarb->setMinimum(0);
    sbCarb->setMaximum(100);
    sbCarb->setMaximumWidth(60);
    gl->addWidget(sbCarb,row,1);
    row++;

    gl->addWidget(new QLabel(tr("Variation, %")),row,0);
    sbVariation = new QDoubleSpinBox;
    sbVariation->setMinimum(0);
    sbVariation->setMaximum(10);
    sbVariation->setDecimals(1);
    sbVariation->setMaximumWidth(60);
    sbVariation->setPrefix("± ");
    gl->addWidget(sbVariation,row,1);
    row++;


    gl->addWidget(new QLabel(tr("Ideal weight, kg")),row,0); //Идеальный вес, кг
    lbNormWeight = new QLabel;
    gl->addWidget(lbNormWeight,row,1);
    row++;

    gl->addWidget(lbDate = new QLabel(/*tr("As of:")*/),row,0,1,2, Qt::AlignHCenter);//По состоянию на:
    row++;

    gl->addWidget(new QLabel(tr("Weight, kg")),row,0);
    lbWeight = new QLabel;
    gl->addWidget(lbWeight,row,1);
    row++;

    QLabel* lb = new QLabel(tr("Body mass index"));
    lb->setToolTip(Indicators::noteTableImb());
    gl->addWidget(lb,row,0);
    lbIndex = new QLabel;
    lbIndex->setToolTip(Indicators::noteTableImb());
    gl->addWidget(lbIndex,row,1);
    row++;

    pbUpdate = new QPushButton(tr("Accept changes"));//Принять изменения
    pbUpdate->setEnabled(false);
    gl->addWidget(pbUpdate,row,0);
    pbClose = new QPushButton(tr("Close unchanged")); //Закрыть без изменений
    gl->addWidget(pbClose,row,1);

    setLayout(gl);

    connect(leName, SIGNAL(textChanged(QString)), this, SLOT(setEnabledPBtUpdate()));
    connect(deDOB, SIGNAL(dateChanged(QDate)), this, SLOT(setEnabledPBtUpdate()));
    connect(sbHeight, SIGNAL(valueChanged(int)), this, SLOT(setEnabledPBtUpdate()));
    connect(cbSex, SIGNAL(currentIndexChanged(int)), this, SLOT(setEnabledPBtUpdate()));
    connect(cbActive, SIGNAL(currentIndexChanged(int)), this, SLOT(setEnabledPBtUpdate()));
    connect(sbProt, SIGNAL(valueChanged(int)), this, SLOT(setEnabledPBtUpdate()));
    connect(sbFat, SIGNAL(valueChanged(int)), this, SLOT(setEnabledPBtUpdate()));
    connect(sbCarb, SIGNAL(valueChanged(int)), this, SLOT(setEnabledPBtUpdate()));
    connect(sbVariation, SIGNAL(valueChanged(double)), this, SLOT(setEnabledPBtUpdate()));

    connect(pbUpdate, SIGNAL(clicked()), this, SLOT(accept()));
    connect(pbClose, SIGNAL(clicked()), this, SLOT(reject()));
    setWindowTitle(tr("Personal data"));
    setWindowIcon(*new QIcon(":/resources/person24.png"));
    pbClose->setMaximumWidth(160);
    adjustSize();
    setMaximumHeight(size().height());
    readSettings();
}

/*virtual*/
void PersonDlg::closeEvent(QCloseEvent * event)
{
    writeSettings();
    QWidget::closeEvent(event);
}

void PersonDlg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()),QSettings::IniFormat);
    settings.beginGroup("PersonDlg");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void PersonDlg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()),QSettings::IniFormat);
    settings.beginGroup("PersonDlg");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

void PersonDlg::selectData()
{
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlQuery query1("SELECT * FROM person", db);
    if(query1.next())
    {
        name = query1.value("name").toString();
        leName->setText(name);
        QDate dob = QDate::fromString(query1.value("dob").toString(), Qt::ISODate);
        deDOB->setDate(dob);
        double height = query1.value("height").toDouble();
        sbHeight->setValue(height);
        cbSex->setCurrentIndex(query1.value("sex").toInt());
        cbActive->setCurrentIndex(query1.value("activity").toInt());
        sbProt->setValue(query1.value("prot").toDouble());
        sbFat->setValue(query1.value("fat").toDouble());
        sbCarb->setValue(query1.value("carb").toDouble());
        sbVariation->setValue(query1.value("variate").toDouble());
        pbUpdate->setEnabled(false);
        QSqlQuery query2("SELECT max(mdate), weight FROM days, weight WHERE days.id=weight.days_id", db);
        if(query2.next())
        {
            QDate sdob = query2.value(0).toDate();
            double weight = query2.value(1).toDouble();
            lbDate->setText(tr("<font color=\"#800000\"><b>As of:</b></font> %1").arg(sdob.toString("dd.MM.yyyy")));//По состоянию на
            lbWeight->setText(tr("<font color=\"#0000FF\">%1 kg</font>").arg(weight,0,'f',1));
            double im = Indicators::imb(weight, height);
            QString note = Indicators::noteImb(im);
            QString color = Indicators::colorImb(im);
            lbIndex->setText(tr("<font color=\"%1\">%2    - %3</font>").arg(color).arg(im,0,'f',2).arg(note));
            double w1 = Indicators::wimb(18.5, height);
            double w2 = Indicators::wimb(25.0, height);
            lbNormWeight->setText(tr("<font color=\"#0000FF\">%1 - %2  kg</font>").arg(w1,0,'f',1).arg(w2,0,'f',1));
        }
    }
}

void PersonDlg::updateData()
{
    int result = QMessageBox::question(this, "", tr("Accept change?"), QMessageBox::Yes, QMessageBox::No);//Принять изменения?
    if(result==QMessageBox::Yes)
    {
        QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
        QSqlDatabase::database().transaction();
        QSqlQuery query(db);
        query.exec(QString("UPDATE person SET name = '%1',"
                           "dob = '%2', height = %3, sex = %4, activity = %5,"
                           "prot = %6, fat = %7, carb = %8, variate = %9 "
                           "WHERE name = '%10'")
                   .arg(leName->text())
                   .arg(deDOB->date().toString(Qt::ISODate))
                   .arg(sbHeight->value())
                   .arg(cbSex->currentIndex())
                   .arg(cbActive->currentIndex())
                   .arg(sbProt->value())
                   .arg(sbFat->value())
                   .arg(sbCarb->value())
                   .arg(sbVariation->value())
                   .arg(name));
        QSqlDatabase::database().commit();
    }
}

void PersonDlg::setEnabledPBtUpdate()
{
    pbUpdate->setEnabled(true);
}

/*virtual*/
void PersonDlg::accept ()
{
    updateData();
    QDialog::accept();
}
