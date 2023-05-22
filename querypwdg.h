#ifndef QUERYPWDG_H
#define QUERYPWDG_H

#include <QWidget>

namespace Ui {
class QueryPWdg;
}

class QSqlQueryModel;

class QueryPWdg : public QWidget
{
    Q_OBJECT

public:
    explicit QueryPWdg(QWidget *parent = 0);
    ~QueryPWdg();

public slots:
    void execQuery();
protected:
    void showEvent(QShowEvent *event);
    void changeEvent(QEvent *event);
private:
    Ui::QueryPWdg *ui;
    QSqlQueryModel* model;
    QString formQueryText(int id);
    void settingModelAndTableView(const QString &name, double price);
    void writeSettings();
    void readSettings();
};

#endif // QUERYPWDG_H
