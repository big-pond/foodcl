#ifndef STARTWIZARD_H
#define STARTWIZARD_H

#include <QWizard>
#include <QToolButton>
#include <QDateEdit>

class QLabel;
class QLineEdit;
class QComboBox;
class QSpinBox;
class QCheckBox;

class StartWizard : public QWizard
{
    Q_OBJECT
public:
    explicit StartWizard(QWidget *parent = 0);
    void setDataDir(const QString& val);
    QString getFileName();
    QString getName();
    int getSex();
    QString getDateOfBirth();
    double getHeight();
    QString getStartCalcDate();
    double getWeight();
    int getActivity();
    int getAim();
    double getProtPercent(){return prot_percent;}
    double getFatPercent(){return fat_percent;}
    double getCarbPercent(){return carb_percent;}
    QString getExistingFileName();
    bool isExistingDict();

public slots:
    void accept();
private:
    QString datadir;
    QWizardPage* filePage;
    double prot_percent;
    double fat_percent;
    double carb_percent;
};

class PersonPage : public QWizardPage
{
    Q_OBJECT

public:
    PersonPage(QWidget *parent = 0);

private:

    QLabel* lbName;
    QLineEdit* leName;

    QLabel* lbSex;
    QComboBox* cbSex;

    QLabel* lbDateOfBirth;
    QDateEdit* deDateOfBirth;

    QLabel* lbHeight;
    QSpinBox* sbHeight;

    QLabel* lbStartCalcDate;
    QDateEdit* deStartCalcDate;

    QLabel* lbWeight;
    QSpinBox* sbWeight;

    QLabel* lbActive;
    QComboBox* cbActive;

    QLabel* lbAim;
    QComboBox* cbAim;
};

class FilePage : public QWizardPage
{
    Q_OBJECT
public slots:
    void selectFile();
    void selectExistingFile();

public:
    FilePage(QWidget *parent = 0);
    void setDataDir(const QString& val){datadir = val;}

private:
    QString datadir;

    QLabel *lbFileName;
    QLineEdit* leFileName;
    QToolButton* tbSelectFile;

    QCheckBox* cbExistingDict;
//    QLabel *lbExistingFileName;
    QLineEdit* leExistingFileName;
    QToolButton* tbSelectExistingFile;

};

#endif // STARTWIZARD_H
