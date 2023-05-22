#include "groupeddlg.h"
#include "ui_groupeddlg.h"

GroupEdDlg::GroupEdDlg(QWidget *parent) : QDialog(parent),  ui(new Ui::GroupEdDlg)
{
    ui->setupUi(this);
}

GroupEdDlg::~GroupEdDlg()
{
    delete ui;
}

bool GroupEdDlg::isTopLevel()
{
    return ui->chTopLevel->isChecked();
}

QString GroupEdDlg::getName()
{
    return ui->leName->text();
}

void GroupEdDlg::setEditMode()
{
    ui->chTopLevel->setEnabled(false);
}

void GroupEdDlg::setData(const bool& istoplevel, const QString& name)
{
    ui->chTopLevel->setChecked(istoplevel);
    ui->leName->setText(name);
}

void GroupEdDlg::clear()
{
    ui->chTopLevel->setChecked(false);
    ui->leName->clear();
}
