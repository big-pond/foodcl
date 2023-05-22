#ifndef MASIDXWDG_H
#define MASIDXWDG_H

#include <QWidget>

namespace Ui {
class MasIdxWdg;
}

class MasIdxWdg : public QWidget
{
    Q_OBJECT

public:
    explicit MasIdxWdg(QWidget *parent = 0);
    ~MasIdxWdg();

protected:
    void showEvent(QShowEvent *event);
    void changeEvent(QEvent *event);
private slots:
    void calc();
private:
    Ui::MasIdxWdg *ui;
    void writeSettings();
    void readSettings();
    void selectData();
};

#endif // MASIDXWDG_H
