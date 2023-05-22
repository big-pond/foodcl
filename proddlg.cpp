#include "proddlg.h"
#include "ui_proddlg.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include "def.h"
#include "prodtree.h"

ProdDlg::ProdDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProdDlg)
{
    ui->setupUi(this);
    prodtree = new ProdTree();
    ui->verticalLayout->insertWidget(0, prodtree);
}

ProdDlg::~ProdDlg()
{
    delete ui;
}

int ProdDlg::getProductId()
{
    int id = 0;
    QTreeWidgetItem* item = prodtree->currentItem();
    if(item!=0)
    {
        id = item->text(ID).toInt();
    }
    return id;
}

void ProdDlg::accept()
{
    QTreeWidgetItem* item = prodtree->currentItem();
    if(item!=0)
    {
        int isgroup = item->text(ISGROUP).toInt();
        if(isgroup==0)
        {
            QDialog::accept();
        }
        else
            QMessageBox::warning(this, QApplication::applicationName(),
                                 tr("The group is selected. Select product."), QMessageBox::Ok);

    }
}
