#ifndef PRODDLG_H
#define PRODDLG_H

#include <QDialog>
//Простой диалог выбора продукта
namespace Ui {
class ProdDlg;
}

class ProdTree;

class ProdDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ProdDlg(QWidget *parent = 0);
    ~ProdDlg();
    ProdTree* getProdTree(){return prodtree;}

    int getProductId();
public slots:
    void accept();
private:
    Ui::ProdDlg *ui;
    ProdTree* prodtree;
};

#endif // PRODDLG_H
