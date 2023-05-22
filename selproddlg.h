#ifndef SELPRODDLG_H
#define SELPRODDLG_H

#include <QDialog>

namespace Ui {
class SelProdDlg;
}
class ProdWdg;
class QTreeWidgetItem;
class QShortcut;

class SelProdDlg : public QDialog
{
    Q_OBJECT

public:
    enum Action {AddNoClose, Edit};
    explicit SelProdDlg(QWidget *parent = 0);
    ~SelProdDlg();
    void readData();
    void setProductId(int id);
    void setDaysId(int id) {days_id=id;}
    void setTime(const QString& tm);
    void setWeight(double weight);
    QString getTimeStr();
    int getProductId();
    double getWeight();
public slots:
    void accept();
    void reject();
signals:
    sendProduct(int id, int daysid, QString time, double weight);

protected slots:
    void currentProdChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void setSS00();
    void setMM00();
    void setMM15();
    void setMM30();
    void setMM45();
    void slotShortcutCtrlW(); //Выделение поля ввода веса
//    void slotShortcutCtrlF(); //Выделение поля поиска
    void slotShortcutCtrlT(); //Выделение поля ввода времени
protected:
    void showEvent(QShowEvent *event);
private:
    Ui::SelProdDlg *ui;
    ProdWdg* prodwdg;
    QWidget* wdgFind;
    QShortcut* keyCtrlW;  // объект сочетания клавиш Ctrl + D
//    QShortcut* keyCtrlF;  // объект сочетания клавиш Ctrl + F
    QShortcut* keyCtrlT;  // объект сочетания клавиш Ctrl + T
    Action action;
    int days_id;
//    QPushButton* pbAddProd; //Добавление приема продукта без закрытия диалога
    void writeSettings();
    void readSettings();
    void setMM(const int &mm);
};

#endif // SELPRODDLG_H
