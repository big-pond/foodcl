#ifndef MAINWDG_H
#define MAINWDG_H

#include <QWidget>
#include  <QSqlQueryModel>

namespace Ui {
class MainWdg;
}
class QSplitter;
class SelProdDlg;

class MainWdg : public QWidget
{
    Q_OBJECT

public:
    explicit MainWdg(QWidget *parent = 0);
    ~MainWdg();
    void openDataBase(const QString& fname);
    void execDaysQuery();
public slots:
    void updateMealView();

    void firstDay();
    void prevDay();
    void nextDay();
    void lastDay();
    void addDay();
    void delDay();
    void editDay();
    void addWeight();

    void firstMeal();
    void prevMeal();
    void nextMeal();
    void lastMeal();

    void addMeal();
    void editMeal();
    void appendMeal(int prodid, int daysid, QString mtime, double weight);
    void delMeal();
    void setEditDayButtonVisible(bool val);
    QSplitter* splitter();
signals:
    void updateChart();
protected:
    void changeEvent(QEvent *event);
private:
    Ui::MainWdg *ui;
    QSqlQueryModel daysmodel;
    QSqlQueryModel mealmodel;
    SelProdDlg* selproddlg;

    QString deficit_color;
    QString norm_color;
    QString excess_color;
    double norm_kcal;
    double norm_prot, norm_fat, norm_carb;
    double pm_prot, pm_fat, pm_carb;
    void showNameWeight();
    void execDaySumQuery();
    void execMealQuery(int id, const QString &mdate);
    void execMealSumQuery(int id, const QString& mdate);

    void setDayModelHeaders();
    void setMealModelHeaders();

    void calcNormCPFC(double& kcal, double& prot, double& fat, double& carb,
                      double &pm_prot, double &pm_fat, double &pm_carb);
};

#endif // MAINWDG_H
