#ifndef QUERYPFCWDG_H
#define QUERYPFCWDG_H

#include <QWidget>

namespace Ui {
class QueryPFCWdg;
}

class QSqlQueryModel;

class QueryPFCWdg : public QWidget
{
    Q_OBJECT

public:
    explicit QueryPFCWdg(QWidget *parent = 0);
    ~QueryPFCWdg();
public slots:
    void execQuery();
protected:
    void showEvent(QShowEvent *event);
    void changeEvent(QEvent *event);

private:
    Ui::QueryPFCWdg *ui;
    QSqlQueryModel* model;
    QString formQueryText(const QString &term);
    void settingModelAndTableView(double avcl, double frpr, double frft, double frcr);
    void writeSettings();
    void readSettings();
};

#endif // QUERYPFCWDG_H
