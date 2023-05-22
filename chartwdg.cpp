#include "chartwdg.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include "def.h"
#include "indicators.h"

//#include <QDebug>

ChartWdg::ChartWdg(QWidget *parent) :  QWidget(parent)
{
    ckrange << 1000 << 2000 << 2500 << 4000 << 5000 << 10000;

    wrange << 60 << 80 << 100 << 120 << 140 << 200 << 250 << 300 << 400 << 500;
    wtcount << 7 << 9  << 11  << 7   << 8   << 11  << 11  << 7   << 9  << 11;

    QVBoxLayout* vbl = new QVBoxLayout();
    vbl->setSpacing(0);
    vbl->setContentsMargins(0, 0, 0, 0);

    QSplitter* spl = new QSplitter(Qt::Vertical);
    clrChart = new QChart();
    clrChart->setTitle(tr("Сalories (kcal)")); //Калории
    clrChart->legend()->setAlignment(Qt::AlignBottom);

    cseries = new QLineSeries;
    cseries->setName(tr("Received calories")); //Получено килокалорий
    QPen pen(Qt::blue);
    pen.setWidth(2);
    cseries->setPen(pen);

    m1series = new QLineSeries;
    m1series->setName(tr("Basal metabolic rate")); //Основной обмен
    pen.setColor(Qt::darkRed);
    m1series->setPen(pen);
    m2series = new QLineSeries;
    m2series->setName(tr("Calories consumed "));//Израсходовоно килокалорий
    pen.setColor(Qt::red);
    m2series->setPen(pen);
    clrChart->addSeries(m1series);
    clrChart->addSeries(m2series);
    clrChart->addSeries(cseries);
    caxisX = new QDateTimeAxis;
    caxisX->setFormat("dd-MM-yyyy");
    caxisX->setLabelsAngle(-90);
    clrChart->setAxisX(caxisX, m1series);
    clrChart->setAxisX(caxisX, m2series);
    clrChart->setAxisX(caxisX, cseries);
    caxisY = new QValueAxis;
    clrChart->setAxisY(caxisY, cseries);
    clrChart->setAxisY(caxisY, m1series);
    clrChart->setAxisY(caxisY, m2series);

    weightChart = new QChart();
    weightChart->setTitle(tr("Weight (kg)"));  //Вес
    weightChart->legend()->setAlignment(Qt::AlignBottom);
    wpseries = new QLineSeries;
    wpseries->setName(tr("My weight")); //Мой вес
    pen.setColor(Qt::blue);
    wpseries->setPen(pen);
    w1series = new QLineSeries;
    w1series->setName(tr("Lower limit of normal weight"));  // Минимальный вес
    pen.setColor(Qt::darkRed);
    w1series->setPen(pen);
    w2series = new QLineSeries;
    w2series->setName(tr("Upper limit of normal weight"));  // Максимальный вес
    pen.setColor(Qt::red);
    w2series->setPen(pen);
    weightChart->addSeries(w1series);
    weightChart->addSeries(w2series);
    weightChart->addSeries(wpseries);
    caxisWX = new QDateTimeAxis;
    caxisWX->setFormat("dd-MM-yyyy");
    caxisWX->setLabelsAngle(-90);
    weightChart->setAxisX(caxisWX, wpseries);
    weightChart->setAxisX(caxisWX, w1series);
    weightChart->setAxisX(caxisWX, w2series);
    caxisWY = new QValueAxis;
    weightChart->setAxisY(caxisWY, wpseries);
    weightChart->setAxisY(caxisWY, w1series);
    weightChart->setAxisY(caxisWY, w2series);


    clrView = new QChartView(this);
    clrView->setChart(clrChart);
    weightView = new QChartView(this);
    weightView->setChart(weightChart);
    spl->addWidget(clrView);
    spl->addWidget(weightView);

    vbl->addWidget(spl);
    setLayout(vbl);
//    setGeometry(20,20,500,800);
}

ChartWdg::~ChartWdg()
{
}

void ChartWdg::updateCharts()
{
    buildCalChart();
    buildWeightChart();
}

void ChartWdg::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        retranslateUi();    // переведём окно заново
    }
}

void ChartWdg::retranslateUi()
{
    clrChart->setTitle(tr("Сalories (kcal)")); //Калории
    cseries->setName(tr("Received calories")); //Получено килокалорий
    m1series->setName(tr("Basal metabolic rate")); //Основной обмен
    m2series->setName(tr("Calories consumed "));//Израсходовоно килокалорий
    weightChart->setTitle(tr("Weight (kg)"));  //Вес
    wpseries->setName(tr("My weight")); //Мой вес
    w1series->setName(tr("Lower limit of normal weight"));  // Минимальный вес
    w2series->setName(tr("Upper limit of normal weight"));  // Максимальный вес
}

void ChartWdg::buildCalChart()
{
    QDateTime datemax, datemin;
    cseries->clear();
    m1series->clear();
    m2series->clear();


    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlQuery query(db);

    query.exec(QString("SELECT min(mdate), max(mdate) FROM days"));
    if(query.next())
    {
        datemin = query.value(0).toDateTime();
        datemax = query.value(1).toDateTime();
        //Готовим данные для кривых основного обмена
        double  he = 0, old = 0;
        int sex = 0, activity = 0;
        query.exec("SELECT * FROM person");
        if(query.next())
        {
            he = query.value("height").toDouble();
            QDate dob = query.value("dob").toDate();
            old = Indicators::yearsold(dob);
            sex = query.value("sex").toInt();
            activity = query.value("activity").toInt();
        }

        double coefa = Def::activityData(activity);

        query.exec(QString("SELECT days.mdate AS 'mdate', "
                           "sum(meal.weight*products.calory/100) AS 'kcal', "
                           "(SELECT weight FROM weight WHERE weight.days_id=days.id) AS pweight "
                           "FROM days, meal, products "
                           "WHERE days.id=meal.days_id AND meal.product_id=products.id "
                           "GROUP BY days.mdate ORDER BY days.mdate"));

        qreal weight = 0;
        qreal kcalmax = 0;
        int tcount = 0;
        while(query.next())
        {
            QDateTime xValue;
            xValue.setDate(query.value("mdate").toDate());
            qreal kcal = query.value("kcal").toDouble();
            if(kcalmax<kcal) kcalmax = kcal;
            cseries->append(xValue.toMSecsSinceEpoch(),kcal);

            qreal w = query.value("pweight").toDouble();
            if(w>0) weight = w;
            double basalmetabolism = Indicators::fMaffinJeor(weight, he, old, sex);
            double metabolism1 = basalmetabolism;
            double metabolism2 = basalmetabolism*coefa;
            if(kcalmax<metabolism2) kcalmax = metabolism2;

            m1series->append(xValue.toMSecsSinceEpoch(),metabolism1);
            m2series->append(xValue.toMSecsSinceEpoch(),metabolism2);
            tcount++;
        }
        if(tcount>15) tcount = 15;
        caxisX->setRange(datemin, datemax);
        caxisX->setTickCount(tcount);
//        kcalmax += 100;
        for(int i=0; i<ckrange.count(); i++)
        {
            if(kcalmax<ckrange[i])
            {
                kcalmax = ckrange[i];
                break;
            }
        }
        caxisY->setRange(0,kcalmax);
        caxisY->setTickCount(11);
    }
}

void ChartWdg::buildWeightChart()
{
    QDateTime datemax, datemin;
    wpseries->clear();
    w1series->clear();
    w2series->clear();

    QSqlDatabase db = QSqlDatabase::database(Def::CONN_MAIN);
    QSqlQuery query(db);

    query.exec(QString("SELECT min(mdate), max(mdate) FROM days, weight WHERE  weight.days_id=days.id "));
    if(query.next())
    {
        datemin = query.value(0).toDateTime();
        datemax = query.value(1).toDateTime();
        //Готовим данные для кривых нормального веса
        double he = 0;
        query.exec("SELECT height FROM person");
        if(query.next())
        {
            he = query.value(0).toDouble();
        }
        double wmin = Indicators::wimb(18.5, he);
        double wmax = Indicators::wimb(25.0, he);

        query.exec(QString("SELECT days.mdate AS mdate, weight.weight AS pweight "
                           "FROM days, weight WHERE weight.days_id=days.id ORDER BY days.mdate"));
        double rangemax = 0;
        int tcount = 0;
        while(query.next())
        {
            QDateTime xValue;
            xValue.setDate(query.value("mdate").toDate());
            qreal pweight = query.value("pweight").toDouble();
            wpseries->append(xValue.toMSecsSinceEpoch(),pweight);
            w1series->append(xValue.toMSecsSinceEpoch(), wmin);
            w2series->append(xValue.toMSecsSinceEpoch(), wmax);
            if(rangemax<pweight) rangemax = pweight;
            if(rangemax<wmax) rangemax = wmax;
            tcount++;
        }
        if(tcount>15) tcount = 15;
        rangemax += 10;
        caxisWX->setRange(datemin, datemax);
        caxisWX->setTickCount(tcount);
        for(int i=0; i<wrange.count(); i++)
        {
            if(rangemax<wrange[i])
            {
                rangemax = wrange[i];
                tcount = wtcount[i];
                break;
            }
        }
        caxisWY->setRange(0,rangemax);
        caxisWY->setTickCount(tcount);
    }
}
