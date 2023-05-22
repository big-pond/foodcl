#ifndef GROUPTREE_H
#define GROUPTREE_H

#include <QTreeWidget>

class QSqlDatabase;
class QSqlRecord;

class GroupTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit GroupTree(QWidget *parent = 0);
    bool readData();
    void parseRecord(const QSqlRecord& record, QSqlDatabase& db, QTreeWidgetItem* parent = 0);
signals:
    
public slots:
protected:
    QIcon folderIcon;

    QTreeWidgetItem* createItem(const QSqlRecord& record,
                                QTreeWidgetItem *parentItem);
    QTreeWidgetItem* createItem(const int& id, const int& parentid,
                                const int& isgroup, const QString &nam,
                                QTreeWidgetItem *parentItem);
};

#endif // GROUPTREE_H
