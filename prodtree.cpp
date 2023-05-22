#include "prodtree.h"
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlRecord>
#include "def.h"

//#include <QDebug>

ProdTree::ProdTree(QWidget *parent) :
    QTreeWidget(parent)
{
    QStringList labels;
    labels << tr("Name") << tr("id") << tr("parentid")<< tr("isGroup")
           << tr("kcal") << tr("Proteins")<<tr("Fats")<<tr("Carbohydrates")<< tr("Price per 1kg");
//    Название,parentid,isGroup,ккал,Белки,Жиры,Углеводы,Цена 1кг

//    header()->setResizeMode(QHeaderView::Interactive);
    setHeaderLabels(labels);
    setColumnWidth(NAME, 200);
    setColumnWidth(ID, 30);
    setColumnWidth(PARENTID, 30);
    setColumnWidth(ISGROUP, 30);
    setColumnWidth(CALORIES, 60);
    setColumnWidth(PROT, 60);
    setColumnWidth(FAT, 60);
    setColumnWidth(CARB, 60);
    setColumnWidth(PRICE, 60);

    setColumnHidden(ID, true);
    setColumnHidden(PARENTID, true);
    setColumnHidden(ISGROUP, true);

    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                         QIcon::Normal, QIcon::Off);
    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                         QIcon::Normal, QIcon::On);
//    documentIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));
    documentIcon.addPixmap(QPixmap(":/resources/meal.png"));
}

bool ProdTree::readData()
{
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    bool result = db.isOpen();
    if(result)
    {
        QSqlQuery query("SELECT name, id,  parent_id, isgroup, calory, "
                        "protein, fat, carbohydrate, price1kg "
                        "FROM products WHERE parent_id=0", db);
        while(query.next())
        {
            QSqlRecord rec = query.record();
            parseRecord(rec, db);
        }
        this->sortItems(0,Qt::AscendingOrder);
    }
    return result;
}

void ProdTree::parseRecord(const QSqlRecord& record, QSqlDatabase &db, QTreeWidgetItem* parentItem)
{
    QTreeWidgetItem *item = createItem(record, parentItem);

    QString querytext = tr("SELECT name, id,  parent_id, isgroup, calory, "
                           "protein, fat, carbohydrate, price1kg "
                           "FROM products WHERE parent_id=%1").arg(record.value(ID).toInt());
//enum PROD {NAME, ID, PARENTID, IZGROUP, CALORY, PROT, FAT, CARB, PRICE};
    QSqlQuery query(querytext, db);
    while(query.next())
    {
        QSqlRecord rec = query.record();
        parseRecord(rec, db, item);
    }
}

QTreeWidgetItem* ProdTree::createItem(const QSqlRecord& record,
                                      QTreeWidgetItem *parentItem)
{
    return createItem(record.value(ID).toInt(),
                      record.value(PARENTID).toInt(),
                      record.value(ISGROUP).toInt(),
                      record.value(NAME).toString(),
                      record.value(CALORIES).toDouble(),
                      record.value(PROT).toDouble(),
                      record.value(FAT).toDouble(),
                      record.value(CARB).toDouble(),
                      record.value(PRICE).toDouble(),
                      parentItem
                      );
}

QTreeWidgetItem* ProdTree::createItem(const int& id, const int& parentid,
                                      const int& isgroup, const QString &nam,
                                      const double& calory, const double& prot,
                                      const double& fat, const double& carb,
                                      const double& price1kg, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *item;
    if (parentItem)
        item = new QTreeWidgetItem(parentItem);
    else
        item = new QTreeWidgetItem(this);

    if(isgroup==0)
    {
        item->setIcon(NAME, documentIcon);
        item->setText(CALORIES, QString::number(calory));
        item->setText(PROT, QString::number(prot));
        item->setText(FAT, QString::number(fat));
        item->setText(CARB, QString::number(carb));
        item->setText(PRICE, QString::number(price1kg));
    }
    else
        item->setIcon(NAME, folderIcon);
    item->setText(ID, QString::number(id));
    item->setText(PARENTID, QString::number(parentid));
    item->setText(ISGROUP, QString::number(isgroup));
    item->setText(NAME, nam);

    return item;
}

QTreeWidgetItem*
ProdTree::appendRecord(const int& parentid, const bool& isgroup,
                       const QString &nam, const double& calory,
                       const double& prot, const double& fat,
                       const double& carb, const double& price1kg,
                       QTreeWidgetItem *parentItem)
{
    int isgrint = 0, id;
    QString textquery;
    if(isgroup)
    {
        isgrint = 1;
        textquery = tr("INSERT INTO products "
                       "(parent_id, isgroup, name) "
                       "VALUES (%1, %2, '%3')").arg(parentid).arg(isgrint).arg(nam);
    }
    else
        textquery = tr("INSERT INTO products "
                       "( parent_id, isgroup, name ,calory, "
                       "protein, fat, carbohydrate, price1kg) "
                       "VALUES (%1, %2, '%3', %4, %5, %6, %7, %8)")
               .arg(parentid).arg(isgrint).arg(nam).arg(calory)
               .arg(prot).arg(fat).arg(carb).arg(price1kg);

    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlDatabase::database().transaction();
    QSqlQuery quins(textquery,db);
    QSqlDatabase::database().commit();

    QSqlQuery dumaxid(QString("SELECT max(id) FROM products"),db);
    if(dumaxid.next())
        id = dumaxid.value(0).toInt();

    QSqlQuery qusel(QString("SELECT id FROM products WHERE parent_id=%1 AND "
                            "isgroup=%2 AND name='%3' AND calory=%4")
                    .arg(parentid).arg(isgrint).arg(nam).arg(calory), db);
    if(qusel.next())
        id = qusel.value(0).toInt();
    return createItem(id, parentid, isgrint, nam, calory, prot,
                      fat, carb, price1kg, parentItem);
}

void ProdTree::editRecord(const QString &nam, const double& calory, const double& prot,
                          const double& fat, const double& carb, const double& price1kg,
                          QTreeWidgetItem *editItem)
{
    int id = editItem->text(ID).toInt();
    bool isgr = editItem->text(ISGROUP).toInt() == 1;
    QString textquery;
    if(isgr)
    {
        textquery = tr("UPDATE products SET name='%1' WHERE id=%2").arg(nam)
                .arg(id);
    }
    else
    {
        textquery = tr("UPDATE products SET name='%1', calory=%2, "
                       "protein=%3, fat=%4, carbohydrate=%5, price1kg=%6 WHERE id=%7")
                .arg(nam).arg(calory).arg(prot).arg(fat).arg(carb).arg(price1kg)
                .arg(id);
    }
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlDatabase::database().transaction();
    QSqlQuery quins(textquery, db);
    QSqlDatabase::database().commit();

    editItem->setText(NAME,nam);
    if(!isgr)
    {
        editItem->setText(CALORIES,QString::number(calory));
        editItem->setText(PROT,QString::number(prot));
        editItem->setText(FAT,QString::number(fat));
        editItem->setText(CARB,QString::number(carb));
        editItem->setText(PRICE,QString::number(price1kg));
    }
}

int ProdTree::checkDelete(QTreeWidgetItem* item)
{
    int res = 0;
    if(item!=0)
    {
        if(item->childCount())
            res = 1;
        else
        {
            int id = item->text(ID).toInt();
            QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
            QSqlQuery query(tr("SELECT id FROM account WHERE productid=%1").arg(id),db);
            if(query.next()) res = 2;
        }
    }
    return res;
}

void ProdTree::deleteRecord(QTreeWidgetItem* item)
{
    if(item!=0)
    {
        int id = item->text(ID).toInt();
        QString textquery = tr("DELETE FROM products WHERE id=%1").arg(id);
        QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
        QSqlDatabase::database().transaction();
        QSqlQuery query(textquery, db);
        QSqlDatabase::database().commit();
        delete item;
    }
}

void ProdTree::moveRecord(QTreeWidgetItem* item, const int& newparentid)
{
    QList<QTreeWidgetItem *> lstitems =  findItems(QString::number(newparentid),
                                   Qt::MatchFixedString|Qt::MatchRecursive,ID);
    if(lstitems.count()>0)
    {
        int itemid = item->text(ID).toInt();
        QTreeWidgetItem* newParentItem = lstitems.at(0);
        if(newParentItem!=0)
        {
            QTreeWidgetItem* oldParentItem = item->parent();
            if(oldParentItem!=0)
                oldParentItem->removeChild(item);
            else
            {
                int idx = indexOfTopLevelItem ( item );
                item = takeTopLevelItem(idx);
            }
            newParentItem->addChild(item);

            QString textquery = tr("UPDATE products SET parent_id=%1 WHERE id=%2")
                    .arg(newparentid).arg(itemid);
            QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
            QSqlDatabase::database().transaction();
            QSqlQuery query(textquery, db);
            QSqlDatabase::database().commit();
        }
    }
}

void ProdTree::selectItem(const int& id)
{
    QList<QTreeWidgetItem *> lstitems =  findItems(QString::number(id),
                                   Qt::MatchFixedString|Qt::MatchRecursive,ID);
    if(lstitems.count()>0)
        setCurrentItem(lstitems.at(0));
}

