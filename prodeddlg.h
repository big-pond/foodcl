#ifndef PRODEDDLG_H
#define PRODEDDLG_H

#include <QDialog>

namespace Ui {
class ProdEdDlg;
}

class ProdEdDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ProdEdDlg(QWidget *parent = 0);
    ~ProdEdDlg();
    QString getName();
    double getCalory();
    double getPrace1kg();
    double getProt();
    double getFat();
    double getCarb();
    void clear();

    void setData(const QString& name, const double& calory,
                 const double& prot, const double& fat, const double& carb,
                 const double& price);

private:
    Ui::ProdEdDlg *ui;
    void writeSettings();
    void readSettings();
};

#endif // PRODEDDLG_H
