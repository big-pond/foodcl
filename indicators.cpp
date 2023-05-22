#include "indicators.h"
#include <QObject>

double Indicators::fHarrisonBenedict(const double& w, const double& h,
                                     const double& old, const int& sex)
{
    double val = 0;
    switch(sex)
    {
    case 0: val = 655.1 + 9.6*w + 1.85*h - 4.86*old; break;
    case 1: val = 66.47 + 13.75*w + 5.0*h - 6.74*old; break;
    }
    return val;
}

double Indicators::fMaffinJeor(const double& w, const double& h,
                               const double& old, const int& sex)
{
    double val = 0;
    switch(sex)
    {
    case 0: val = 9.99*w + 6.25*h - 4.92*old - 161; break;
    case 1: val = 9.99*w + 6.25*h - 4.92*old + 5; break;
    }
    return val;
}

double Indicators::imb(const double& w, const double& h)
{
    double H = h/100.0;
    return w/(H*H);
}

double Indicators::wimb(const double& imb, const double& h)
{
    double H = h/100.0;
    return imb*(H*H);
}

QString Indicators::noteImb(const double& im)
{
    QString note;
    if(im<16) note = QObject::tr("Severely underweight");  //Выраженный дефицит массы тела
    else if(im<18.5) note = QObject::tr("Underweight"); //Недостаточная (дефицит) масса тела
    else if(im<25) note = QObject::tr("Normal (healthy weight)"); //Норма
    else if(im<30) note = QObject::tr("Overweight"); //Избыточная масса тела (предожирение)
    else if(im<35) note = QObject::tr("Obese Class I (moderately obese)"); //Ожирение первой степени
    else if(im<40) note = QObject::tr("Obese class II (severely obese)"); //Ожирение второй степени
    else note = QObject::tr("Obese class III (very severely obese)"); //Ожирение третьей степени (морбидное)
    return note;
}

QString Indicators::noteTableImb()
{
/*
    return  QObject::tr("16 и менее - выраженный дефицит массы тела\n"
               "16—18,5 - недостаточная (дефицит) масса тела\n"
               "18,5—25 - норма\n"
               "25—30 - избыточная масса тела (предожирение)\n"
               "30—35 - ожирение первой степени\n"
               "35—40 - ожирение второй степени\n"
               "40 и более - ожирение третьей степени (морбидное)");
*/
    return  QObject::tr("16 and less - severely Underweight\n"
               "16—18,5 - underweight\n"
               "18,5—25 - normal (healthy weight)\n"
               "25—30 - overweight\n"
               "30—35 - obese class I (moderately obese)\n"
               "35—40 - obese class II (severely obese)\n"
               "40 and more - obese class III (very severely obese)");
}

QString Indicators::colorImb(const double& im)
{
    QString color;
    if(im<18.5) color = "#FF00FF";
    else if(im<25) color = "#339933";
    else color = "#FF0000";
    return color;
}

double Indicators::yearsold(const QDate &dob)
{
    QDate now = QDate::currentDate();
    return (now.year() - dob.year()) + (now.month() - dob.month())/12.0 + (now.day() - dob.day())/365.0;
}
