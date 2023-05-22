#ifndef DEF_H
#define DEF_H
#include <QString>
#include <QStringList>

enum PRODFIELD {NAME, ID, PARENTID, ISGROUP, CALORIES, PROT, FAT, CARB, PRICE};


class Def
{
public:
    static const QString APP_NAME;

    static const QString CONN_MAIN;
    static const QString CONN_1;

    static QString datadir;
    static QString querydir;
    static QString filename;
    //Нормы потребления (калории, б,ж,у)
    static const double default_pfc_ratio[3][3];
    static double pfc_ratio[3][3];
    static const double default_pfc_variation;
    static double pfc_variation;

    static int language_id;
    static bool showbuttonstabdays;
    static bool dialogprodinput; //Ввод приема пищи true - в диалоговом окне, false - множественный

    static void setDefaultPFCRatio();

    static QString languageName(int i);
    static QString languageSuffix(int i);
    static int languageCount();

    static QString activityName(int i);
    static double activityData(int i);
    static int activityCount();

    static QString aimName(int i);
    static void aimData(int i, double &prot, double &fat, double &carb);
    static int aimCount();

    static double approximateToUp(double val, int digit);
    static bool odd(int val);
private:
    static QStringList language_list;
    static int activity_count;
    static int aim_count;
//    static QStringList activitylist;
//    static QList<double> coefactivitylist;

};

#endif // DEF_H
