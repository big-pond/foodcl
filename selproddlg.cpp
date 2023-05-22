#include "selproddlg.h"
#include "ui_selproddlg.h"
#include <QPushButton>
#include <QSettings>
#include <QLineEdit>
#include <QShortcut>
#include <QDialogButtonBox>
#include <QMessageBox>
#include "def.h"
#include "prodwdg.h"
#include "prodtree.h"


//#include <QDebug>

SelProdDlg::SelProdDlg(QWidget *parent) :  QDialog(parent), ui(new Ui::SelProdDlg)
{
    ui->setupUi(this);
    QGridLayout* gl = ui->gridLayout;
    prodwdg = new ProdWdg;
    gl->addWidget(prodwdg,0,0, 1,7);

    wdgFind = prodwdg->releaseWdgFind();
    gl->addWidget(wdgFind,2,0,1,7);

    connect(ui->tbMM00, SIGNAL(clicked()), this, SLOT(setMM00()));
    connect(ui->tbMM15, SIGNAL(clicked()), this, SLOT(setMM15()));
    connect(ui->tbMM30, SIGNAL(clicked()), this, SLOT(setMM30()));
    connect(ui->tbMM45, SIGNAL(clicked()), this, SLOT(setMM45()));
    connect(prodwdg->getProdTree(), SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            SLOT(currentProdChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    keyCtrlW = new QShortcut(this); // Инициализируем объект
    keyCtrlW->setKey(Qt::CTRL + Qt::Key_W); // Устанавливаем сочетание клавиш
    connect(keyCtrlW, SIGNAL(activated()), this, SLOT(slotShortcutCtrlW()));
//    keyCtrlF = new QShortcut(this);
//    keyCtrlF->setKey(Qt::CTRL + Qt::Key_F);
//    connect(keyCtrlF, SIGNAL(activated()), this, SLOT(slotShortcutCtrlF()));
    keyCtrlT = new QShortcut(this);
    keyCtrlT->setKey(Qt::CTRL + Qt::Key_T);
    connect(keyCtrlT, SIGNAL(activated()), this, SLOT(slotShortcutCtrlT()));

    readSettings();
}

SelProdDlg::~SelProdDlg()
{
    writeSettings();
    if(wdgFind!=0) delete wdgFind;
    delete ui;
}

/*virtual*/
void SelProdDlg::accept ()
{
    QLineEdit* leFind = prodwdg->get_leFind();// dynamic_cast<QLineEdit*>(wdgFind);
    QTreeWidgetItem* item = prodwdg->getProdTree()->currentItem();
    if(item!=0)
    {
        if(action==SelProdDlg::Edit) //Диалог был открыт для редактирования
        {
            int isgroup = item->text(ISGROUP).toInt();
            if(isgroup==0)
            {
                if(leFind) leFind->clear();
                QDialog::accept();
            }
        }
        else if (action==SelProdDlg::AddNoClose) //Диалог был открыт для многократного добавления
        {
            int isgroup = item->text(ISGROUP).toInt();
            if(isgroup==0)
            {
                emit sendProduct(prodwdg->getProductId(),
                                 days_id,
                                 ui->timeEdit->time().toString(),
                                 ui->sbWeight->value());
                if(leFind) leFind->clear();
            }
            else
                QMessageBox::warning(this, QApplication::applicationName(),
                                     tr("The product group is selected."), QMessageBox::Ok); //Выбрана группа продуктов.
        }
    }
    else
        QMessageBox::warning(this, QApplication::applicationName(),
                             tr("Product (dish) not selected."), QMessageBox::Ok);//Не выбран продукт (блюдо)
}

void SelProdDlg::reject()
{
    QLineEdit* leFind = prodwdg->get_leFind();//dynamic_cast<QLineEdit*>(wdgFind);
    if(leFind) leFind->clear();
    QDialog::reject();
}

void SelProdDlg::showEvent(QShowEvent *event)
{
    QPushButton* btCancel = ui->buttonBox->button(QDialogButtonBox::Cancel);
    QPushButton* btOk = ui->buttonBox->button(QDialogButtonBox::Ok);
    if(action == SelProdDlg::AddNoClose)
    {
        btCancel->setText(tr("Close"));
        btOk->setText(tr("Append"));
    }
    else //SelProdDlg::Edit
    {
        btCancel->setText(tr("Cancel"));
        btOk->setText(tr("OK"));
    }
    QDialog::showEvent(event);
}

void SelProdDlg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()),QSettings::IniFormat);
    settings.beginGroup("SelProdDlg");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void SelProdDlg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()),QSettings::IniFormat);
    settings.beginGroup("SelProdDlg");
    bool geom = settings.contains("geometry");
    if (geom)
        restoreGeometry(settings.value("geometry").toByteArray());
    else
        setGeometry(5,25,444, 575);
    settings.endGroup();
}

void SelProdDlg::readData()
{
    prodwdg->getProdTree()->readData();
}

void SelProdDlg::setProductId(int id)
{
    prodwdg->setProductId(id);
    if(id>0)
    {
        action = SelProdDlg::Edit; //Редактирование записи - выбор продукта, изменение времени и веса
        ui->lbActInfo->setText(tr("<font color=\"#000080\"><b>Editing a record - selecting a product, changing the time and weight</b></font>"));
    }
    else
    {
        action = SelProdDlg::AddNoClose; //Новая запись - выбор продукта, ввод времени и количества
        ui->lbActInfo->setText(tr("<font color=\"#000080\"><b>New record - product selection, time and weight input</b></font>"));
    }
}
void SelProdDlg::setTime(const QString& tm)
{
    ui->timeEdit->setTime(QTime::fromString(tm));
}

void SelProdDlg::setWeight(double weight)
{
    ui->sbWeight->setValue(weight);
}


void SelProdDlg::currentProdChanged(QTreeWidgetItem * current, QTreeWidgetItem * /*previous*/)
{
    int isgroup = current->data(3,0).toInt();
    if(isgroup==0)
    {
        ui->lbProd->setText(tr("<html><head/><body><p><span style=\" font-size:9pt; color:#4800FF; "
                                    "background: #FFF8D0;\">%1, %2 kcal per 100 g</span></p></body></html>")
                        .arg(current->data(0,0).toString())
                        .arg(current->data(4,0).toString()));
    }
    else
        ui->lbProd->setText(QString("<html><head/><body><p><span style=\" font-size:9pt; color:#4800FF; "
                                    "background: #FFF8D0;\"> - </span></p></body></html>"));
}

int SelProdDlg::getProductId()
{
    return prodwdg->getProductId();
}

QString SelProdDlg::getTimeStr()
{
    setSS00();
    return ui->timeEdit->time().toString();
}

double SelProdDlg::getWeight()
{
    return ui->sbWeight->value();
}

void SelProdDlg::setSS00()
{
    QTime t = ui->timeEdit->time();
    int h = t.hour();
    int m = t.minute();
    t.setHMS(h,m,0);
    ui->timeEdit->setTime(t);
}
void SelProdDlg::setMM(const int& mm)
{
    QTime t = ui->timeEdit->time();
    int h = t.hour();
    t.setHMS(h,mm,0);
    ui->timeEdit->setTime(t);
}

void SelProdDlg::setMM00()
{
    setMM(0);
}

void SelProdDlg::setMM15()
{
    setMM(15);
}

void SelProdDlg::setMM30()
{
    setMM(30);
}

void SelProdDlg::setMM45()
{
    setMM(45);
}

void SelProdDlg::slotShortcutCtrlW()
{
    ui->sbWeight->setFocus();
    ui->sbWeight->selectAll();
}

//void SelProdDlg::slotShortcutCtrlF()
//{
//    wdgFind->setFocus();
//}

void SelProdDlg::slotShortcutCtrlT()
{
    ui->timeEdit->setFocus();
}
