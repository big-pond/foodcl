#include "mainwdg.h"
#include "ui_mainwdg.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QScrollBar>
#include <QSplitter>
#include <QDate>
#include <QInputDialog>

#include "def.h"
#include "indicators.h"
#include "selproddlg.h"
#include "datedlg.h"

//#include <QDebug>

enum {Days_Id, Days_Mdate, Days_Weight, Days_Ccal, Days_P, Days_F, Days_C, Days_MyWeight};
enum MealCol {Meal_Id, Meal_Time, Meal_ProdId, Meal_Prod, Meal_Weight, Meal_PrCcal, Meal_Ccal,
              Meal_P, Meal_F, Meal_C};

MainWdg::MainWdg(QWidget *parent) :  QWidget(parent),
    ui(new Ui::MainWdg)//, conname("test")
{
    ui->setupUi(this);
    selproddlg = 0;
    deficit_color = "#0000bb";
    norm_color = "#007000";
    excess_color = "#dd0000";
    ui->tvDays->setModel(&daysmodel);
    ui->tvDays->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvDays->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvDays->setAlternatingRowColors(true);
    connect(ui->tvDays->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(updateMealView()));

    ui->tvMeal->setModel(&mealmodel);
    ui->tvMeal->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvMeal->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvMeal->setAlternatingRowColors(true);

    connect(ui->tbFirstDay, SIGNAL(clicked()),SLOT(firstDay()));
    connect(ui->tbPrevDay, SIGNAL(clicked()),SLOT(prevDay()));
    connect(ui->tbNextDay, SIGNAL(clicked()),SLOT(nextDay()));
    connect(ui->tbLastDay, SIGNAL(clicked()),SLOT(lastDay()));
    connect(ui->tbAddDay, SIGNAL(clicked()),SLOT(addDay()));
    connect(ui->tbAddWeight, SIGNAL(clicked()), SLOT(addWeight()));
    connect(ui->tbDelDay, SIGNAL(clicked()), SLOT(delDay()));
    connect(ui->tbEditDay, SIGNAL(clicked()), SLOT(editDay()));

    connect(ui->tbFirstMeal, SIGNAL(clicked()),SLOT(firstMeal()));
    connect(ui->tbPrevMeal, SIGNAL(clicked()),SLOT(prevMeal()));
    connect(ui->tbNextMeal, SIGNAL(clicked()),SLOT(nextMeal()));
    connect(ui->tbLastMeal, SIGNAL(clicked()),SLOT(lastMeal()));
    connect(ui->tbAddMeal, SIGNAL(clicked()), SLOT(addMeal()));
    connect(ui->tbEditMeal, SIGNAL(clicked()), SLOT(editMeal()));
    connect(ui->tbDelMeal, SIGNAL(clicked()), SLOT(delMeal()));

    setEditDayButtonVisible(false);
}

MainWdg::~MainWdg()
{
    if(selproddlg!=0) delete selproddlg;
    delete ui;
}

void MainWdg::openDataBase(const QString &fname)
{
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    db.setDatabaseName(fname);
    if (!db.open())
    {
        QMessageBox::information(this,"",tr("The new database is not open."));//Новая база данных не открыта.
        return;
    }
    if(selproddlg!=0) //Проверяем так как справочник продуктов уже другой
    {
        delete selproddlg;
        selproddlg = 0;
    }
    execDaysQuery();
    ui->tvDays->setCurrentIndex(daysmodel.index(daysmodel.rowCount()-1,1));
    ui->tvDays->verticalScrollBar()->setValue(ui->tvDays->verticalScrollBar()->maximum());
    showNameWeight();
    emit updateChart();
}

void MainWdg::execDaysQuery()
{
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlQuery query(db);

    query.exec("SELECT days.id, days.mdate, "
              "(SELECT sum(meal.weight) FROM meal WHERE meal.days_id=days.id) AS mweight, "
              "round((SELECT sum(meal.weight*products.calory/100.0) FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id),0) AS ccal, "
              "round((SELECT sum(meal.weight*products.protein/100.0) FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id),1) AS prot, "
              "round((SELECT sum(meal.weight*products.fat/100.0) FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id),1) AS fats, "
              "round((SELECT sum(meal.weight*products.carbohydrate/100.0) FROM meal, products WHERE meal.days_id=days.id AND meal.product_id=products.id),1) AS carb, "
              "(SELECT weight FROM weight WHERE weight.days_id=days.id) AS pweight "
              "FROM days ORDER BY days.mdate");
    daysmodel.setQuery(query);
    setDayModelHeaders();
    while (daysmodel.canFetchMore()) //Чтобы в tvDays выводились все строки, а не 256
           daysmodel.fetchMore();
    ui->tvDays->setColumnHidden(Days_Id, true);
    ui->tvDays->resizeColumnsToContents();
    ui->tvDays->resizeRowsToContents();

    calcNormCPFC(norm_kcal, norm_prot, norm_fat, norm_carb, pm_prot, pm_fat, pm_carb);
    QString s = tr("Norm:<br><font color=\"%1\">"
                   "&nbsp;&nbsp; 1200-%2 kcal<br>"
                   "&nbsp;&nbsp; P: %3±%4 g<br>"
                   "&nbsp;&nbsp; F: %5±%6 g<br>"
                   "&nbsp;&nbsp; C: %7±%8 g.</font><br>"
                   "<font color=\"%9\">█ This color shows values that are <b>less</b> than the norm;</font><br>"
                   "<font color=\"%10\">█ this color shows values that <b>correspond</b> to the norm;</font><br>"
                   "<font color=\"%11\">█ this color shows values that <b>exceed</b> the norm.</font>"
                  ).arg(norm_color).arg(norm_kcal,0,'f',0)
                   .arg(norm_prot,0,'f',0).arg(pm_prot,0,'f',1)
                   .arg(norm_fat,0,'f',0).arg(pm_fat,0,'f',1)
                   .arg(norm_carb,0,'f',0).arg(pm_carb,0,'f',1)
                   .arg(deficit_color).arg(norm_color).arg(excess_color);
    ui->lbDaySum->setToolTip(s);
    ui->lbMealSum->setToolTip(s);
    execDaySumQuery();
}

void MainWdg::setDayModelHeaders()
{
    daysmodel.setHeaderData(Days_Mdate, Qt::Horizontal, tr("Date")); //Дата
    daysmodel.setHeaderData(Days_Weight, Qt::Horizontal, tr("Eaten\nweight,\ng")); //Вес\nсъеденного,\nг
    daysmodel.setHeaderData(Days_Ccal, Qt::Horizontal, tr("Calorie\nintake,\nkcal"));//Калорийность\nсъеденного,\nккал
    daysmodel.setHeaderData(Days_P, Qt::Horizontal, tr("P,\ng"));// Б,г
    daysmodel.setHeaderData(Days_F, Qt::Horizontal, tr("F,\ng"));// Ж,г
    daysmodel.setHeaderData(Days_C, Qt::Horizontal, tr("C,\ng"));// У,г
    daysmodel.setHeaderData(Days_MyWeight, Qt::Horizontal, tr("My\nweight,\nkg"));//Мой\nвес,\nкг
}

void MainWdg::execMealQuery(int id, const QString& mdate)
{
    QSqlQuery query(QSqlDatabase::database(Def::CONN_MAIN));

    query.exec(QString("SELECT meal.id, meal.mtime, meal.product_id,"
                      "products.name, meal.weight, products.calory, "
                      "round(products.calory*meal.weight/100.0, 1) as ccal, "
                      "round(products.protein*meal.weight/100.0, 1) as prot, "
                      "round(products.fat*meal.weight/100.0, 1) as fat, "
                      "round(products.carbohydrate*meal.weight/100.0, 1) as carb "
                      "FROM meal, products WHERE meal.product_id=products.id AND meal.days_id=%1 "
                      "ORDER BY meal.mtime").arg(id));
    mealmodel.setQuery(query);
    setMealModelHeaders();
    ui->tvMeal->setColumnHidden(Meal_Id, true);
    ui->tvMeal->setColumnHidden(Meal_ProdId, true);
    ui->tvMeal->resizeColumnsToContents();
    ui->tvMeal->resizeRowsToContents();
    execMealSumQuery(id, mdate);
}

void MainWdg::setMealModelHeaders()
{
    mealmodel.setHeaderData(Meal_Time, Qt::Horizontal, tr("Time")); //Время
    mealmodel.setHeaderData(Meal_Prod, Qt::Horizontal, tr("Product (dish) name"));//Продукт (блюдо)
    mealmodel.setHeaderData(Meal_Weight, Qt::Horizontal, tr("Weight,\ng"));//Вес, г
    mealmodel.setHeaderData(Meal_PrCcal, Qt::Horizontal, tr("Calorie content\nper 100 g,\nkcal"));//Калорийность 100 г, ккал
    mealmodel.setHeaderData(Meal_Ccal, Qt::Horizontal, tr("Calorie\ncontent,\n kcal"));//Калорийность,\n ккал
    mealmodel.setHeaderData(Meal_P, Qt::Horizontal, tr("P,\ng"));// Б,г
    mealmodel.setHeaderData(Meal_F, Qt::Horizontal, tr("F,\ng"));// Ж,г
    mealmodel.setHeaderData(Meal_C, Qt::Horizontal, tr("C,\ng"));// У,г
}

void MainWdg::calcNormCPFC(double &kcal, double &prot, double &fat, double &carb,
                           double &pm_prot, double &pm_fat, double &pm_carb)
{
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlQuery query("SELECT * FROM person", db);
    if(query.next())
    {
        QDate dob = QDate::fromString(query.value("dob").toString(), Qt::ISODate);
        double old = Indicators::yearsold(dob);
        double h = query.value("height").toDouble();
        int sex = query.value("sex").toInt();
        int i = query.value("activity").toInt();
        double prot_norm = query.value("prot").toDouble()/100;
        double fat_norm = query.value("fat").toDouble()/100;
        double carb_norm = query.value("carb").toDouble()/100;
        double plus_minus = query.value("variate").toDouble()/100;   //например +- 2.5% от нормы
        double coefa = Def::activityData(i);
        double w =60;
        QSqlQuery query2("SELECT max(mdate), weight FROM days, weight WHERE days.id=weight.days_id", db);
        if(query2.next())
            w = query2.value("weight").toDouble();
        double kmj = Indicators::fMaffinJeor(w,h,old,sex);
        kcal = coefa*kmj;
        prot = kcal*prot_norm/4;
        fat = kcal*fat_norm/9;
        carb = kcal*carb_norm/4;
        pm_prot = kcal*plus_minus/4;
        pm_fat = kcal*plus_minus/9;
        pm_carb = kcal*plus_minus/4;
//        qDebug() << prot << "+-" << pm_prot;
//        qDebug() << fat << "+-" << pm_fat;
//        qDebug() << carb << "+-" << pm_carb;
    }
}

void MainWdg::execDaySumQuery()
{
    double sumw = 0, sumc = 0, sum_p = 0, sum_f = 0, sum_c = 0;
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlQuery qu("SELECT sum(meal.weight) as W, "
           "sum(meal.weight*products.calory/100.0),"
           "sum(meal.weight*products.protein/100.0),"
           "sum(meal.weight*products.fat/100.0),"
           "sum(meal.weight*products.carbohydrate/100.0) "
           "FROM meal, products WHERE meal.product_id=products.id",db);
    if(qu.next())
    {
        sumw = qu.value(0).toDouble();
        sumc = qu.value(1).toDouble();
        sum_p = qu.value(2).toDouble();
        sum_f = qu.value(3).toDouble();
        sum_c = qu.value(4).toDouble();

        if(daysmodel.rowCount()>0)
        {
            sumw /= daysmodel.rowCount();
            sumc /= daysmodel.rowCount();
            sum_p /= daysmodel.rowCount();
            sum_f /= daysmodel.rowCount();
            sum_c /= daysmodel.rowCount();
        }
    }
    QString kc_color = norm_color;
    QString p_color = norm_color;
    QString f_color = norm_color;
    QString c_color = norm_color;
    if (sumc<1200.0) kc_color = deficit_color;
    else if (sumc>norm_kcal) kc_color = excess_color;
    if (sum_p<(norm_prot-pm_prot)) p_color = deficit_color;
    else if (sum_p>(norm_prot+pm_prot)) p_color = excess_color;
    if (sum_f<(norm_fat-pm_fat)) f_color = deficit_color;
    else if (sum_f>(norm_fat+pm_fat)) f_color = excess_color;
    if (sum_c<(norm_carb-pm_carb)) c_color = deficit_color;
    else if (sum_c>(norm_carb+pm_carb)) c_color = excess_color;
    QString s = tr("<span style=\"font-style:italic;\">Daily average:</span>"
                   " %1 g"
                   "<font color=\"%2\">&nbsp;&nbsp; %3 kcal </font>"
                   "<font color=\"%4\">&nbsp;&nbsp; P: %5 g</font>"
                   "<font color=\"%6\">&nbsp;&nbsp; F: %7 g</font>"
                   "<font color=\"%8\">&nbsp;&nbsp; C: %9 g</font>")
            .arg(sumw,0,'f',1)
            .arg(kc_color).arg(sumc,0,'f',1)
            .arg(p_color).arg(sum_p,0,'f',1)
            .arg(f_color).arg(sum_f,0,'f',1)
            .arg(c_color).arg(sum_c,0,'f',1);
    ui->lbDaySum->setText(s);
}

void MainWdg::execMealSumQuery(int id, const QString& mdate)
{
    double sumw = 0, sumkc = 0, sum_p = 0, sum_f = 0, sum_c = 0;
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlQuery qu(db);
    qu.exec(QString("SELECT sum(meal.weight) AS w, "
                    "sum(meal.weight*products.calory/100) AS kc "
                    "FROM meal, products WHERE meal.product_id=products.id  "
                    "AND meal.days_id=%1")
            .arg(id));
    qu.exec(QString("SELECT sum(meal.weight) AS w, "
           "sum(meal.weight*products.calory/100.0) AS kc, "
           "sum(meal.weight*products.protein/100.0) AS prot, "
           "sum(meal.weight*products.fat/100.0) AS fats, "
           "sum(meal.weight*products.carbohydrate/100.0) AS carb "
           "FROM meal, products WHERE meal.product_id=products.id "
           "AND meal.days_id=%1").arg(id));
    if(qu.next())
    {
        sumw = qu.value("w").toDouble();
        sumkc = qu.value("kc").toDouble();
        sum_p = qu.value("prot").toDouble();
        sum_f = qu.value("fats").toDouble();
        sum_c = qu.value("carb").toDouble();
    }
    QString kc_color = norm_color;
    QString p_color = norm_color;
    QString f_color = norm_color;
    QString c_color = norm_color;
    if (sumkc<1200.0) kc_color = deficit_color;
    else if (sumkc>norm_kcal) kc_color = excess_color;
    if (sum_p<(norm_prot-pm_prot)) p_color = deficit_color;
    else if (sum_p>(norm_prot+pm_prot)) p_color = excess_color;
    if (sum_f<(norm_fat-pm_fat)) f_color = deficit_color;
    else if (sum_f>(norm_fat+pm_fat)) f_color = excess_color;
    if (sum_c<(norm_carb-pm_carb)) c_color = deficit_color;
    else if (sum_c>(norm_carb+pm_carb)) c_color = excess_color;
    QString s = tr("<span style=\"font-style:italic;\">Total %1:</span>"
                   "&nbsp;&nbsp; %2 g"
                   "<font color=\"%3\">&nbsp;&nbsp; %4 kcal </font>"
                   "<font color=\"%5\">&nbsp;&nbsp; P: %6 g</font>"
                   "<font color=\"%7\">&nbsp;&nbsp; F: %8 g</font>"
                   "<font color=\"%9\">&nbsp;&nbsp; C: %10 g</font>"
                   )
            .arg(mdate)
            .arg(sumw,0,'f',1)
            .arg(kc_color).arg(sumkc,0,'f',1)
            .arg(p_color).arg(sum_p,0,'f',1)
            .arg(f_color).arg(sum_f,0,'f',1)
            .arg(c_color).arg(sum_c,0,'f',1);
    ui->lbMealSum->setText(s);
}

void MainWdg::updateMealView()
{
    QModelIndex index = ui->tvDays->currentIndex();
    QSqlRecord record = daysmodel.record(index.row());
    int id = record.value("id").toInt();
    QString mdate = record.value("mdate").toString();
    ui->lbDate->setText(tr("<html><head/><body><p><span style=\" font-size:10pt; font-weight:600; color:#aa0000;\">%1</span></p></body></html>").arg(mdate));
    execMealQuery(id, mdate);
}

void MainWdg::firstDay()
{
    QModelIndex index = daysmodel.index(0,1);
    if(index.isValid())
        ui->tvDays->setCurrentIndex(index);
}

void MainWdg::prevDay()
{
    QModelIndex index = ui->tvDays->currentIndex();
    QModelIndex previndex = index.sibling(index.row()-1, index.column());
    if(previndex.isValid())
        ui->tvDays->setCurrentIndex(previndex);
}

void MainWdg::nextDay()
{
    QModelIndex index = ui->tvDays->currentIndex();
    QModelIndex nextindex = index.sibling(index.row()+1, index.column());
    if(nextindex.isValid())
        ui->tvDays->setCurrentIndex(nextindex);
}

void MainWdg::lastDay()
{
    QModelIndex index = daysmodel.index(daysmodel.rowCount()-1,1);
    if(index.isValid())
        ui->tvDays->setCurrentIndex(index);
}

void MainWdg::addDay()
{
    if(daysmodel.rowCount())
    {
        QSqlRecord record = daysmodel.record(daysmodel.rowCount()-1);
        QDate date = record.value("mdate").toDate();
        QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
        QSqlQuery query(db);
        QSqlDatabase::database().transaction();
        query.exec(QString("INSERT INTO days (mdate) VALUES ('%1')")
                   .arg(date.addDays(1).toString(Qt::ISODate)));
        QSqlDatabase::database().commit();
        execDaysQuery();
        ui->tvDays->setCurrentIndex(daysmodel.index(daysmodel.rowCount()-1, 1));
    }
}

void MainWdg::delDay()
{
    QModelIndex i = ui->tvDays->currentIndex();
    if(i.isValid())
    {
        QDate date = daysmodel.data(daysmodel.index(i.row(), Days_Mdate)).toDate();
        int days_id = daysmodel.data(daysmodel.index(i.row(), Days_Id)).toInt();
        QSqlQuery qu(QSqlDatabase::database(Def::CONN_MAIN));
        qu.exec(QString("SELECT weight.id, weight.weight FROM weight WHERE weight.days_id=%1").arg(days_id));
        bool weight_is = qu.next(); //есть запись о весе на эту дату
        qu.exec(QString("SELECT COUNT(*) FROM meal WHERE meal.days_id=%1").arg(days_id));
        int count = 0;
        if(qu.next())
            count = qu.value(0).toInt();
        if(count>0)
        {
            QMessageBox::information(this,"",tr("I can 't delete the date %1 because there are records of meals.").arg(date.toString("yyyy-MM-dd")));//Новая база данных не открыта.
            return;
        }
        else
        {

            int res = QMessageBox::question(this,"",tr("Delete date %1?").arg(date.toString("yyyy-MM-dd")));
            if (res==QMessageBox::Yes)
            {
                QSqlDatabase::database().transaction();
                qu.exec(QString("DELETE FROM days WHERE id = %1").arg(days_id));
                if(weight_is)
                    qu.exec(QString("DELETE FROM weight WHERE days_id = %1").arg(days_id));
                QSqlDatabase::database().commit();
                execDaysQuery();
                int row = i.row();
                if(row>daysmodel.rowCount()-1) row = daysmodel.rowCount()-1;
                ui->tvDays->setCurrentIndex(daysmodel.index(row, 1));
                emit updateChart();
            }
        }
    }
}

void MainWdg::editDay()
{
    QModelIndex i = ui->tvDays->currentIndex();
    if(i.isValid())
    {//Внимание! Неправильное изменение даты может привести к нарушению данных. Изменить дату?
        int res = QMessageBox::question(this,"",tr("Attention! Changing the date incorrectly may result in a data breach. Change the date?"));
        if(res==QMessageBox::No) return;
        QDate date = daysmodel.data(daysmodel.index(i.row(), Days_Mdate)).toDate();
        int days_id = daysmodel.data(daysmodel.index(i.row(), Days_Id)).toInt();
        int x = ui->tvDays->columnViewportPosition(Days_Mdate);
        int y = ui->tvDays->rowViewportPosition(i.row());
        DateDlg* dlg = new DateDlg();
        dlg->move(x,y);
        dlg->setDate(date);
        if(dlg->exec()==QDialog::Accepted)
        {
            date = dlg->getDate();
            QSqlQuery qu(QSqlDatabase::database(Def::CONN_MAIN));
            qu.exec(QString("UPDATE days SET mdate='%1' WHERE id=%2").arg(date.toString(Qt::ISODate)).arg(days_id));
            execDaysQuery();
            ui->tvDays->setCurrentIndex(i);
            emit updateChart();
        }
        delete dlg;
    }
    else
        QMessageBox::information(this,"",tr("No date selected."));

}

void MainWdg::addWeight()
{
    QModelIndex i = ui->tvDays->currentIndex();
    if(i.isValid())
    {
        bool ok = false;
        QDate date = daysmodel.data(daysmodel.index(i.row(), Days_Mdate)).toDate();
        int days_id = daysmodel.data(daysmodel.index(i.row(), Days_Id)).toInt();
        QSqlQuery qu(QSqlDatabase::database(Def::CONN_MAIN));
        qu.exec(QString("SELECT weight.id, weight.weight FROM weight WHERE weight.days_id=%1").arg(days_id));
        if(qu.next()) //Уже есть запись о весе на эту дату
        {
            int id = qu.value(0).toInt();
            double w = qu.value(1).toDouble();
            w = QInputDialog::getDouble(this,"",tr("Change your weight on %1").arg(date.toString("yyyy-MM-dd")), w, 10, 400,1,&ok);//Измените значение веса на %1
            if(ok)
                qu.exec(QString("UPDATE weight SET weight=%1 WHERE weight.id=%2").arg(w).arg(id));
        }
        else //Записи о весе на эту дату нет
        {
            qu.exec("SELECT min(weight) FROM weight");
            double w = 0;
            if(qu.next())
                w = qu.value(0).toDouble();
            w = QInputDialog::getDouble(this,"",tr("Enter your weight on %1").arg(date.toString("yyyy-MM-dd")), w, 10, 400,1,&ok);//Введите вес на %1
            if(ok)
                qu.exec(QString("INSERT INTO weight (weight, days_id) VALUES (%1, %2)").arg(w).arg(days_id));
        }
        if(ok)
        {
            execDaysQuery();
            ui->tvDays->setCurrentIndex(i);
            showNameWeight();
            emit updateChart();
        }
    }
}

void MainWdg::firstMeal()
{
    QModelIndex index = mealmodel.index(0,1);
    if(index.isValid())
        ui->tvMeal->setCurrentIndex(index);
}

void MainWdg::prevMeal()
{
    QModelIndex index = ui->tvMeal->currentIndex();
    QModelIndex previndex = index.sibling(index.row()-1, index.column());
    if(previndex.isValid())
        ui->tvMeal->setCurrentIndex(previndex);
}

void MainWdg::nextMeal()
{
    QModelIndex index = ui->tvMeal->currentIndex();
    QModelIndex nextindex = index.sibling(index.row()+1, index.column());
    if(nextindex.isValid())
        ui->tvMeal->setCurrentIndex(nextindex);
}

void MainWdg::lastMeal()
{
    QModelIndex index = mealmodel.index(mealmodel.rowCount()-1,1);
    if(index.isValid())
        ui->tvMeal->setCurrentIndex(index);
}

void MainWdg::addMeal()
{
    if(selproddlg==0)
    {
        selproddlg = new SelProdDlg();
        selproddlg->readData();
        connect(selproddlg, SIGNAL(sendProduct(int, int, QString, double)), this, SLOT(appendMeal(int, int, QString, double)));
    }
    QModelIndex i = ui->tvDays->currentIndex();
    if(i.isValid())
    {
        selproddlg->setDaysId(daysmodel.data(daysmodel.index(i.row(),0)).toInt());
        selproddlg->setProductId(-1);
        selproddlg->exec();
    }
}

void MainWdg::appendMeal(int prodid, int daysid, QString mtime, double weight)
{
    int mrow = mealmodel.rowCount();
    QModelIndex d_idx = ui->tvDays->currentIndex();

    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlDatabase::database().transaction();
    QSqlQuery query(db);
    query.exec(QString("INSERT INTO meal (mtime, weight, product_id, days_id) "
                       "VALUES ('%1', %2, %3, %4)")
               .arg(mtime).arg(weight).arg(prodid).arg(daysid));
    QSqlDatabase::database().commit();
    execDaysQuery();
    ui->tvDays->setCurrentIndex(d_idx);
    updateMealView();
    ui->tvMeal->setCurrentIndex(mealmodel.index(mrow,1));
    emit updateChart();
}

void MainWdg::editMeal()
{
    QModelIndex d_idx = ui->tvDays->currentIndex();
    if(!d_idx.isValid()) return;
    QModelIndex m_idx = ui->tvMeal->currentIndex();
    if(!m_idx.isValid()) return;
    QSqlRecord record = mealmodel.record(m_idx.row());
    int id = record.value("id").toInt();
    QString mtime = record.value("mtime").toString();
    int productid = record.value("product_id").toInt();
    double weight = record.value("weight").toDouble();

    if(selproddlg==0)
    {
        selproddlg = new SelProdDlg();
        selproddlg->readData();
        connect(selproddlg, SIGNAL(sendProduct(int, int, QString, double)), this, SLOT(appendMeal(int, int, QString, double)));
    }

    selproddlg->setTime(mtime);
    selproddlg->setProductId(productid);
    selproddlg->setWeight(weight);

    if(selproddlg->exec()==QDialog::Accepted)
    {
        QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
        QSqlQuery query(db);
        QSqlDatabase::database().transaction();
        query.exec(QString("UPDATE meal SET mtime = '%1', product_id = %2, weight =  %3 WHERE id = %4")
                        .arg(selproddlg->getTimeStr())
                        .arg(selproddlg->getProductId()).arg(selproddlg->getWeight())
                        .arg(id));
        QSqlDatabase::database().commit();

        execDaysQuery();
        ui->tvDays->setCurrentIndex(d_idx);
        updateMealView();
        ui->tvMeal->setCurrentIndex(m_idx);
        emit updateChart();
    }
}

void MainWdg::delMeal()
{
    QModelIndex d_idx = ui->tvDays->currentIndex();
    if(!d_idx.isValid()) return;
    QModelIndex m_idx = ui->tvMeal->currentIndex();
    if(!m_idx.isValid()) return;
    int rows = mealmodel.rowCount();
    int row = m_idx.row();
    QSqlRecord record = mealmodel.record(row);
    int id = record.value("id").toInt();

    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlQuery query(db);

    QSqlDatabase::database().transaction();
    query.exec(QString("DELETE FROM meal WHERE id = %1").arg(id));
    QSqlDatabase::database().commit();

    execDaysQuery();
    ui->tvDays->setCurrentIndex(d_idx);
    updateMealView();
    if(row==rows-1)
        m_idx = m_idx.sibling(row-1, m_idx.column());;
    ui->tvMeal->setCurrentIndex(m_idx);
    emit updateChart();
}

void MainWdg::setEditDayButtonVisible(bool val)
{
    ui->tbEditDay->setVisible(val);
    ui->tbDelDay->setVisible(val);
}

QSplitter *MainWdg::splitter()
{
    return ui->splitter;
}

void MainWdg::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        //Следующие компоненты при изменении языка не меняются. Запомним их содержимое.
        QString uname = ui->lbUName->text();
        QString date = ui->lbDate->text();

        ui->retranslateUi(this);    // переведём окно заново
        setDayModelHeaders();
        setMealModelHeaders();
        QModelIndex curindex = ui->tvDays->currentIndex();
        execDaysQuery(); //Для обновления суммарных данных
        ui->tvDays->setCurrentIndex(curindex);

        //Следующие компоненты при изменении языка не меняются. Восстановим их содержимое
        ui->lbUName->setText(uname);
        ui->lbDate->setText(date);
    }
}

void MainWdg::showNameWeight()
{
    ui->lbUName->setText("");
    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlQuery query1("SELECT name FROM person", db);
    if(query1.next())
    {
        QSqlQuery query2("SELECT max(mdate), weight FROM days, weight WHERE days.id=weight.days_id", db);
        if(query2.next())
        {
            ui->lbUName->setText(tr("<html><head/><body><p>"
                "<span style=\" font-size:10pt; font-weight:600; color:#aa0000;\">"
                "%1 - %2 kg</span></p></body></html>")
                .arg(query1.value(0).toString())
                .arg(query2.value(1).toString()));
        }
    }
}
