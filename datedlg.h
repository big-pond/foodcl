#ifndef DATEDLG_H
#define DATEDLG_H

#include <QDialog>

namespace Ui {
class DateDlg;
}

class DateDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DateDlg(QWidget *parent = 0);
    ~DateDlg();
    void setDate(const QDate& date);
    QDate getDate();

private:
    Ui::DateDlg *ui;
};

#endif // DATEDLG_H
