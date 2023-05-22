#ifndef PRODWDG_H
#define PRODWDG_H

#include <QWidget>

namespace Ui {
class ProdWdg;
}

class ProdTree;
class QTreeWidgetItem;
class QShortcut;
class QLineEdit;

class ProdWdg : public QWidget
{
    Q_OBJECT

public:
    explicit ProdWdg(QWidget *parent = 0);
    ~ProdWdg();
    void setProductId(const int& id);
    int getProductId();
    ProdTree* getProdTree(){return prodtree;}
    QWidget *releaseWdgFind();
    QLineEdit* get_leFind();


public slots:
    void appendProduct();
    void appengGroup();
    void editRecord();
    void deleteRecord();
    void moveRecord();
    void findItems(const QString&);
    void goToNextFind();
    void slotShortcutCtrlF(); //Выделение поля поиска
private:
    Ui::ProdWdg *ui;
    ProdTree* prodtree;
    int currow_in_finditemlist;
    QList<QTreeWidgetItem*> finditemlist;
    QShortcut* keyCtrlN;  // объект сочетания клавиш Ctrl + N
    QShortcut* keyCtrlF;  // объект сочетания клавиш Ctrl + F
};

#endif // PRODWDG_H
