#include "prodwdg.h"
#include "ui_prodwdg.h"
#include <QMessageBox>
#include <QShortcut>
#include "def.h"
#include "prodeddlg.h"
#include "groupeddlg.h"
#include "groupdlg.h"
#include "prodtree.h"
#include <QDebug>

ProdWdg::ProdWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProdWdg)
{
    ui->setupUi(this);
    prodtree = new ProdTree();
    layout()->addWidget(prodtree);
    connect(ui->tbAdd, SIGNAL(clicked()), this, SLOT(appendProduct()));
    connect(ui->tbAddGroup, SIGNAL(clicked()), this,SLOT(appengGroup()));
    connect(ui->tbEdit, SIGNAL(clicked()), this, SLOT(editRecord()));
    connect(ui->tbDel, SIGNAL(clicked()), this, SLOT(deleteRecord()));
    connect(ui->tbMove,SIGNAL(clicked()), this, SLOT(moveRecord()));

    connect(ui->leFind, SIGNAL(textChanged(QString)), this, SLOT(findItems(QString)));
    connect(ui->tbNextFind, SIGNAL(clicked()), this, SLOT(goToNextFind()));

    keyCtrlN = new QShortcut(this); // Инициализируем объект
    keyCtrlN->setKey(Qt::CTRL + Qt::Key_N); // Устанавливаем сочетание клавиш
    connect(keyCtrlN, SIGNAL(activated()), this, SLOT(goToNextFind()));

    keyCtrlF = new QShortcut(this);
    keyCtrlF->setKey(Qt::CTRL + Qt::Key_F);
    connect(keyCtrlF, SIGNAL(activated()), this, SLOT(slotShortcutCtrlF()));

    ui->leFind->setFocus();
    ui->tbNextFind->setEnabled(false);

    setWindowTitle(tr("Product (dish)"));//Продукт (блюдо)
    setWindowIcon(*new QIcon(":/resources/apple.png"));
}

ProdWdg::~ProdWdg()
{
    if(testAttribute(Qt::WA_DeleteOnClose))
    {
//      qDebug() << "Delete on close";
    }
    delete ui;
}

void ProdWdg::setProductId(const int& id)
{
    prodtree->selectItem(id);
}

int ProdWdg::getProductId()
{
    int id = 0;
    QTreeWidgetItem* item = prodtree->currentItem();
    if(item!=0)
    {
        id = item->text(ID).toInt();
    }
    return id;
}

void ProdWdg::appendProduct()
{
    QTreeWidgetItem* item = prodtree->currentItem();
    ProdEdDlg* pdlg = new ProdEdDlg();
    pdlg->setWindowTitle(tr("Add product (dish)"));//Добавить продукт (блюдо)
    int parentid = 0;
    if(item!=0)
    {
        if(item->text(ISGROUP).toInt()==0)
            item = item->parent();
        if(item!=0)
            parentid = item->text(ID).toInt();
    }
    if(pdlg->exec()==QDialog::Accepted)
    {
        prodtree->appendRecord(parentid,
                               0,
                               pdlg->getName(),
                               pdlg->getCalory(),
                               pdlg->getProt(),
                               pdlg->getFat(),
                               pdlg->getCarb(),
                               pdlg->getPrace1kg(),
                               item);
    }
    delete pdlg;
}

void ProdWdg::appengGroup()
{
    QTreeWidgetItem* item = prodtree->currentItem();
    GroupEdDlg* gdlg = new GroupEdDlg(this);
    gdlg->setWindowTitle(tr("Add product group"));//Добавить группу
    if(gdlg->exec()==QDialog::Accepted)
    {
        int parentid = 0;
        if(gdlg->isTopLevel())
        {
            item = 0;
        }
        else
        {
            if(item!=0)
                if(item->text(ISGROUP).toInt()==0)
                    item = item->parent();
            if(item!=0)
                parentid = item->text(ID).toInt();
        }
        prodtree->appendRecord(parentid, 1, gdlg->getName(), 0, 0, 0, 0, 0, item);
    }
    delete gdlg;
}

void ProdWdg::editRecord()
{
    QTreeWidgetItem* item = prodtree->currentItem();
    if(item!=0)
    {
        bool isgr = item->text(ISGROUP).toInt() == 1;
        if(isgr)
        {
            GroupEdDlg* gdlg = new GroupEdDlg(this);
            gdlg->setData(item->parent()==0, item->text(NAME));
            gdlg->setEditMode();
            gdlg->setWindowTitle(tr("Edit name group"));//Редактировать название группы
            if(gdlg->exec()==QDialog::Accepted)
            {
                prodtree->editRecord(gdlg->getName(), 0, 0, 0, 0, 0, item);
            }
            delete gdlg;
        }
        else
        {
            ProdEdDlg* pdlg = new ProdEdDlg();
            pdlg->setWindowTitle(tr("Edit product (dish)"));//Редактировать продукт (блюдо)
            pdlg->setData(item->text(NAME),
                          item->text(CALORIES).toDouble(),
                          item->text(PROT).toDouble(),
                          item->text(FAT).toDouble(),
                          item->text(CARB).toDouble(),
                          item->text(PRICE).toDouble()
                         );
            //pdlg->setEditMode();
            if(pdlg->exec()==QDialog::Accepted)
            {
                prodtree->editRecord(pdlg->getName(),
                                     pdlg->getCalory(),
                                     pdlg->getProt(),
                                     pdlg->getFat(),
                                     pdlg->getCarb(),
                                     pdlg->getPrace1kg(),
                                     item);
            }
            delete pdlg;
        }
    }
    else
        QMessageBox::warning(this, "", tr("No record selected."), QMessageBox::Ok);//Не выбрана запись.
}

void ProdWdg::deleteRecord()
{
    QTreeWidgetItem* item = prodtree->currentItem();
    if(item!=0)
    {
        int check = prodtree->checkDelete(item);
        if(check==1)
            QMessageBox::warning(this, "",tr("The group has products (dishes). Deletion is not possible."),
                                 QMessageBox::Ok);//В группе есть продукты (блюда). Удаление невозможно.
        else if(check==2)
            QMessageBox::warning(this, "", tr("The product (dish) is used in accounting. Deletion is not possible."),
                                 QMessageBox::Ok); //Продукт используется в учете. Удаление невозможно.
        else
        {
        int res = QMessageBox::question(this,"",tr("Delete record?"),QMessageBox::Yes|QMessageBox::No);//Удалить запись
        if(res==QMessageBox::Yes)
            prodtree->deleteRecord(item);
        }
    }
    else
        QMessageBox::warning(this, "",tr("No record selected."), QMessageBox::Ok);//Не выбрана запись.
}

void ProdWdg::moveRecord()
{
    QTreeWidgetItem* item = prodtree->currentItem();
    if(item!=0)
    {
        GroupDlg* gdlg = new GroupDlg(this);
        gdlg->readData();
        if(gdlg->exec()==QDialog::Accepted)
        {
            int newparentid = gdlg->getSelectedGroupID();
            if(newparentid==-1)
                QMessageBox::warning(this, "",tr("No group selected. Moving is not possible."),QMessageBox::Ok);//Не выбрана группа. Перемещение невозможно.
            else
                prodtree->moveRecord(item, newparentid);
        }
        delete gdlg;
    }
    else
        QMessageBox::warning(this, "", tr("No record selected."), QMessageBox::Ok);
}

void ProdWdg::findItems(const QString& text)
{
    if (text.isEmpty())
    {
        ui->tbNextFind->setEnabled(false);
        currow_in_finditemlist = -1;
        ui->lbVarCount->setText("0");
        ui->lbVar->setText("0");
    }
    else
    {
        finditemlist = prodtree->findItems(text,
                                           Qt::MatchContains|Qt::MatchRecursive, 0);
        if(finditemlist.count()>0)
        {
            prodtree->setCurrentItem(finditemlist.at(0));
            currow_in_finditemlist = 0;
            ui->tbNextFind->setEnabled(true);
            ui->lbVarCount->setText(QString::number(finditemlist.count()));
            ui->lbVar->setText(QString::number(currow_in_finditemlist+1));
        }
    }
}

void ProdWdg::goToNextFind()
{
    if (finditemlist.count()==0)
    {
        currow_in_finditemlist = -1;
        return;
    }
    if (currow_in_finditemlist<finditemlist.count()-1)
        currow_in_finditemlist++;
    else if (currow_in_finditemlist==finditemlist.count()-1)
        currow_in_finditemlist = 0;
    prodtree->setCurrentItem(finditemlist.at(currow_in_finditemlist));
    ui->lbVar->setText(QString::number(currow_in_finditemlist+1));

}

QWidget *ProdWdg::releaseWdgFind()
{
    layout()->removeWidget(ui->wdgFind);
    return ui->wdgFind;
}

QLineEdit *ProdWdg::get_leFind()
{
    return ui->leFind;
}

void ProdWdg::slotShortcutCtrlF()
{
    ui->leFind->setFocus();
}
