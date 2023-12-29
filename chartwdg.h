#ifndef CHARTWDG_H
#define CHARTWDG_H

#include <QWidget>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
class QLineSeries;
class QDateTimeAxis;
class QValueAxis;
QT_CHARTS_END_NAMESPACE
QT_CHARTS_USE_NAMESPACE

class ChartWdg : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWdg(QWidget *parent = 0);
    ~ChartWdg();

public slots:
    void updateCharts();
protected:
    void changeEvent(QEvent *event);
private:
    QChart* clrChart;
    QLineSeries *cseries;
    QLineSeries *cmseries;
    QLineSeries *m1series;
    QLineSeries *m2series;
    QDateTimeAxis *caxisX;
    QValueAxis *caxisY;
    QChartView* clrView;

    QChart* weightChart;
    QLineSeries *wpseries;
    QLineSeries *w1series;
    QLineSeries *w2series;
    QDateTimeAxis *caxisWX;
    QValueAxis *caxisWY;
    QChartView* weightView;
    QList<double> ckrange;
    QList<double> wrange;
    QList<double> wtcount;

    void retranslateUi();
    void buildCalChart();
    void buildWeightChart();
};

#endif // CHARTWDG_H
