#include "def.h"
#include <QObject>

const QString Def::APP_NAME = "foodcl";

const QString Def::CONN_MAIN = "connection_main";
const QString Def::CONN_1 = "connection_1";

QString Def::datadir = "./data";
QString Def::querydir = "./queryes";
QString Def::filename = "./data/demo.sl3";
//Соотношение БЖУ и отклонение (2.5% по умолчанию)
const double Def::default_pfc_ratio[3][3] = {{ 20, 30, 30 },
                                             { 20, 30, 20 },
                                             { 60, 40, 50 }};
double Def::pfc_ratio[3][3] = {{ 20, 30, 30 },
                               { 20, 30, 20 },
                               { 60, 40, 50 }};
const double Def::default_pfc_variation = 2.5;
double Def::pfc_variation = 2.5;

int Def::language_id = -1;
bool Def::showbuttonstabdays = false; //не запоминать значение в ini (включаются только на сеанс)

bool Def::dialogprodinput = true; //Ввод приема пищи true - в диалоговом окне, false - множественный

QStringList Def::language_list = QStringList() << "English;en_US"
                                               << "Русский;ru_RU";

int Def::activity_count = 5;

int Def::aim_count = 3;
/*
QStringList Def::activitylist = QStringList() << QObject::tr("Sedentary lifestyle")
                                              << QObject::tr("Small (sports 1-3 days a week)")
                                              << QObject::tr("Medium (sport 3-5 days a week)")
                                              << QObject::tr("High (sport 6-7 days a week)")
                                              << QObject::tr("Very high (physical work, 2 workouts per day)");

QList<double> Def::coefactivitylist = QList<double>() << 1.2
                                                      << 1.375
                                                      << 1.55
                                                      << 1.725
                                                      << 1.9;
*/
void Def::setDefaultPFCRatio()
{
    for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
            Def::pfc_ratio[i][j] = Def::default_pfc_ratio[i][j];
}

QString Def::languageName(int i)
{
    return Def::language_list[i].split(";")[0];
}

QString Def::languageSuffix(int i)
{
    return Def::language_list[i].split(";")[1];
}

int Def::languageCount()
{
    return Def::language_list.count();
}

QString Def::activityName(int i)
{
    QString s;
    if(i<0) i=0;
    if(i>Def::activity_count-1) i=Def::activity_count-1;
    switch(i)
    {
    case 0: s = QObject::tr("Sedentary lifestyle"); break;
    case 1: s = QObject::tr("Small (sports 1-3 days a week)"); break;
    case 2: s = QObject::tr("Medium (sport 3-5 days a week)"); break;
    case 3: s = QObject::tr("High (sport 6-7 days a week)"); break;
    case 4: s = QObject::tr("Very high (physical work, 2 workouts per day)"); break;
    }
    return s;
}

double Def::activityData(int i)
{
    double val = 0;
    if(i<0) i=0;
    if(i>Def::activity_count-1) i=Def::activity_count-1;
    switch(i)
    {
    case 0: val = 1.2;   break;
    case 1: val = 1.375; break;
    case 2: val = 1.55;  break;
    case 3: val = 1.725; break;
    case 4: val = 1.9;   break;
    }
    return val;
}

int Def::activityCount()
{
    return  Def::activity_count;
}

QString Def::aimName(int i)
{
    QString s;
    if(i<0) i=0;
    if(i>Def::aim_count-1) i=Def::aim_count-1;
    switch(i)
    {
    case 0: s = QObject::tr("Losing weight"); break; //Похудание
    case 1: s = QObject::tr("Maintaining a normal weight"); break;
    case 2: s = QObject::tr("Gaining muscle mass"); break;  //Набор мышечной массы
    }
    return s;
}

void Def::aimData(int i, double& prot, double& fat, double& carb)
{
    if(i<0) i=0;
    if(i>Def::activity_count-1) i=Def::activity_count-1;
    switch(i)
    {
    case 0: prot = 20; fat = 20; carb = 60; break;
    case 1: prot = 30; fat = 30; carb = 40; break;
    case 2: prot = 30; fat = 20; carb = 50; break;
    }
}

int Def::aimCount()
{
    return  Def::aim_count;
}

double Def::approximateToUp(double val, int digit)
{
    double mult = pow(10,digit);
    double x=floor(val/mult);
    return (x+1)*mult;
}

bool Def::odd(int val)
{
    return (val&1);
}
