#ifndef PRODTREE_H
#define PRODTREE_H

#include <QTreeWidget>

class QSqlDatabase;
class QSqlRecord;

class ProdTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit ProdTree(QWidget *parent = 0);

    bool readData();
    void parseRecord(const QSqlRecord& record, QSqlDatabase& db, QTreeWidgetItem* parent = 0);


    QTreeWidgetItem* appendRecord(const int& parentid, const bool& isgroup,
                                  const QString &nam, const double& calory, const double& prot,
                                  const double& fat, const double& carb,
                                  const double& price1kg, QTreeWidgetItem *parentItem);
    void editRecord(const QString &nam, const double& calory,
                    const double& prot, const double& fat,
                    const double& carb, const double& price1kg,
                    QTreeWidgetItem *editItem);
    int checkDelete(QTreeWidgetItem* item);
    void deleteRecord(QTreeWidgetItem* item);
    void moveRecord(QTreeWidgetItem* item, const int& newparentid);
    void selectItem(const int& id);
    
protected:
    QIcon folderIcon;
    QIcon documentIcon;

    QTreeWidgetItem* createItem(const QSqlRecord& record,
                                QTreeWidgetItem *parentItem);
    QTreeWidgetItem* createItem(const int& id, const int& parentid,
                                const int& isgroup, const QString &nam,
                                const double& calory, const double& prot,
                                const double& fat, const double& carb,
                                const double& price1kg, QTreeWidgetItem *parentItem);
};

#endif // PRODTREE_H
