#include "grouptree.h"
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlRecord>
#include "def.h"

GroupTree::GroupTree(QWidget *parent) :
    QTreeWidget(parent)
{
    QStringList labels;
    labels << tr("Product group name") << "id" << "parentid" << "isGroup"; //название группы продуктов

    setHeaderLabels(labels);
    setColumnWidth(NAME, 200);
    setColumnWidth(ID, 30);
    setColumnWidth(PARENTID, 30);
    setColumnWidth(ISGROUP, 30);

    setColumnHidden(ID, true);
    setColumnHidden(PARENTID, true);
    setColumnHidden(ISGROUP, true);

    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                         QIcon::Normal, QIcon::Off);
    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                         QIcon::Normal, QIcon::On);
}
bool GroupTree::readData()
{
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    bool result = db.isOpen();
    if(result)
    {
        QSqlQuery query("SELECT name, id,  parent_id, isgroup, calory, price1kg, "
                        "protein, fat, carbohydrate "
                        "FROM products WHERE isgroup=1 AND parent_id=0", db);
        while(query.next())
        {
            QSqlRecord rec = query.record();
            parseRecord(rec, db);
        }
        this->sortItems(0,Qt::AscendingOrder);
    }
    return result;
}

void GroupTree::parseRecord(const QSqlRecord& record, QSqlDatabase& db, QTreeWidgetItem* parentItem)
{
    QTreeWidgetItem *item = createItem(record, parentItem);

    QString querytext = QString("SELECT name, id,  parent_id, isgroup "
                                "FROM products WHERE isgroup=1 AND parent_id=%1")
            .arg(record.value(ID).toInt());
    //{NAME, ID, PARENTID, IZGROUP, CALORY, PRICE1KG, PROT, FAT, CARB};
    QSqlQuery query(querytext, db);
    while(query.next())
    {
        QSqlRecord rec = query.record();
        parseRecord(rec, db, item);
    }
}

QTreeWidgetItem* GroupTree::createItem(const QSqlRecord& record,
                                       QTreeWidgetItem *parentItem)
{
    return createItem(record.value(ID).toInt(),
                      record.value(PARENTID).toInt(),
                      record.value(ISGROUP).toInt(),
                      record.value(NAME).toString(),
                      parentItem
                      );
}

QTreeWidgetItem* GroupTree::createItem(const int& id, const int& parentid,
                                      const int& isgroup, const QString &nam,
                                      QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *item;
    if (parentItem)
        item = new QTreeWidgetItem(parentItem);
    else
        item = new QTreeWidgetItem(this);
    item->setIcon(NAME, folderIcon);
    item->setText(NAME, nam);
    item->setText(ID, QString::number(id));
    item->setText(PARENTID, QString::number(parentid));
    item->setText(ISGROUP, QString::number(isgroup));

    return item;
}
