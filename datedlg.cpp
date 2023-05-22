#include "datedlg.h"
#include "ui_datedlg.h"

DateDlg::DateDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DateDlg)
{
    ui->setupUi(this);
}

DateDlg::~DateDlg()
{
    delete ui;
}

void DateDlg::setDate(const QDate &date)
{
    ui->dateEdit->setDate(date);
}

QDate DateDlg::getDate()
{
    return ui->dateEdit->date();
}
