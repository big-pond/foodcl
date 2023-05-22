#include "groupdlg.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include "def.h"

GroupDlg::GroupDlg(QWidget *parent) : QDialog(parent)
{
    grouptree = new GroupTree();
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);

    setLayout(new QVBoxLayout);
    layout()->addWidget(grouptree);
    layout()->addWidget(buttonBox);
    layout()->setContentsMargins(2,2,2,2);
    layout()->setSpacing(2);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
//    setWindowTitle(tr("Product Group Selection")); //  Выбор группы продуктов
}

void GroupDlg::readData()
{
    grouptree->readData();
}

int GroupDlg::getSelectedGroupID()
{
    int id = -1;
    QTreeWidgetItem* item = grouptree->currentItem();
    if(item!=0)
    {
        id = item->text(ID).toInt();
    }
    return id;
}

QList<int> GroupDlg::getListGroupID()
{
    QList<int> lstid;
    QTreeWidgetItem* item = grouptree->currentItem();
    if(item!=0)
    {
        int id = item->text(ID).toInt();
        lstid.append(id);
        readChildID(&lstid, item);
    }
    return lstid;
}

void GroupDlg::setCurrentTopLevelFirstItem()
{
    QTreeWidgetItem * item = grouptree->topLevelItem(0);
    if(item!=0) grouptree->setCurrentItem(item);
}

void GroupDlg::readChildID(QList<int> *lstid, QTreeWidgetItem* item)
{
    int count  = item->childCount();
    for(int i=0; i<count; i++)
    {
        QTreeWidgetItem* childitem = item->child(i);
        int id = childitem->text(ID).toInt();
        lstid->append(id);
        readChildID(lstid, childitem);
    }
}
