#ifndef QUERYWDG_H
#define QUERYWDG_H

#include <QWidget>

namespace Ui {
class QueryWdg;
}

class QSqlQueryModel;

class QueryWdg : public QWidget
{
    Q_OBJECT

public:
    explicit QueryWdg(QWidget *parent = 0);
    ~QueryWdg();

public slots:
    void execQuery();
protected:
    void showEvent(QShowEvent *event);
    void changeEvent(QEvent *event);
private:
    Ui::QueryWdg *ui;
    QSqlQueryModel* model;
    void settingModelAndTableView();
    QString term1;
    QString term2;
    QString formQueryGrpText(QList<int> idlist);
    QString formQueryAllText();
    void execQueryGrpSum();
    void execQueryAllSum();
    void writeSettings();
    void readSettings();
};

#endif // QUERYWDG_H
