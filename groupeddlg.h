#ifndef GROUPEDDLG_H
#define GROUPEDDLG_H

#include <QDialog>

namespace Ui {
class GroupEdDlg;
}

class GroupEdDlg : public QDialog
{
    Q_OBJECT

public:
    explicit GroupEdDlg(QWidget *parent = 0);
    ~GroupEdDlg();
    bool isTopLevel();
    QString getName();
    void setEditMode();
    void clear();
    void setData(const bool& istoplevel, const QString& name);

private:
    Ui::GroupEdDlg *ui;
};

#endif // GROUPEDDLG_H
