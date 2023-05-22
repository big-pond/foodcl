#ifndef GROUPDLG_H
#define GROUPDLG_H

#include <QDialog>
#include "grouptree.h"

class QDialogButtonBox;

class GroupDlg : public QDialog
{
    Q_OBJECT
public:
    explicit GroupDlg(QWidget *parent = 0);
    void readData();
    int getSelectedGroupID();
    QList<int> getListGroupID();
    void setCurrentTopLevelFirstItem();

private:
    GroupTree* grouptree;
    QDialogButtonBox* buttonBox;
    void readChildID(QList<int> *lstid, QTreeWidgetItem* item);
};

#endif // GROUPDLG_H

