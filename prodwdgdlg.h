#ifndef PRODWDGDLG_H
#define PRODWDGDLG_H

#include <QDialog>

namespace Ui {
class ProdwdgDlg;
}
class ProdWdg;

class ProdwdgDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ProdwdgDlg(QWidget *parent = 0);
    ~ProdwdgDlg();

    void readData();
private:
    Ui::ProdwdgDlg *ui;
    ProdWdg* prodwdg;
    void writeSettings();
    void readSettings();
};

#endif // PRODWDGDLG_H
