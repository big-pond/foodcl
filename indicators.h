#ifndef INDICATORS_H
#define INDICATORS_H

#include <QString>
#include <QDate>

class Indicators //Статический класс
{
public:
    //Формула Харриса-Бенедикта для расчета базальной энергетической потребности
    //Женщины: 655,1 + 9,6 * w + 1,85 * h - 4,68 * old,
    //Мужчины: 66,47 + 13,75 * w + 5,0 * h - 6,74 * old,
    //w - вес, кг
    //h - рост, см
    //old - возраст, лет

    //sex - пол: 0-женский, 1-мужской
    static double fHarrisonBenedict(const double& w, const double& h, const double& old,
                             const int& sex);


    //Формула для подсчета калорий (формула Маффина-Джеора)
    //Одной из наиболее точных формул подсчета ежедневных калорий считается формула Маффина-Джеора.
    //Основной обмен (ОО) – это столько калорий сколько нужно организму на
    //поддержание жизнедеятельности в случае полного покоя (дыхание, перекачка крови,
    //рост волос и т.д).
    //Для женщин : ОО = 9,99 * w + 6.25 * h - 4,92 * old - 161,
    //Для мужчин : ОО = 9,99 * w + 6.25 * h - 4,92 * old + 5, где
    //w - вес, кг
    //h - рост, см
    //old - возраст, лет

    //sex - пол: 0-женский, 1-мужской
   static  double fMaffinJeor(const double& w, const double& h, const double& old,
                       const int& sex);


    //Индекс массы тела I = w / (H * H), где
    //w - вес, кг
    //H = h/100 - рост, м
    //h - рост, см
    static double imb(const double& w, const double& h);

    //Для вычисления веса по индексу массы тела и росту
    static double wimb(const double& imb, const double& h);

    static QString noteImb(const double& im);
    static QString noteTableImb();
    static QString colorImb(const double& im);
    //Вычисление возраста по дате рождения
    static double yearsold(const QDate& dob);
};

#endif // INDICATORS_H
