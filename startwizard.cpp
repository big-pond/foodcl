#include "startwizard.h"
#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

#include "def.h"

//#include <QDebug>

StartWizard::StartWizard(QWidget *parent) :
    QWizard(parent)
{
    addPage(new PersonPage);
    filePage = new FilePage;
    addPage(filePage);

 //   setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/background.png"));

    setWindowTitle(tr("New calorie accounting"));
}

void StartWizard::accept()
{
//Всевозможные проверки правильности ввода данных
    QString myname = field("myName").toString();
    int i = field("aim").toInt();
    Def::aimData(i, prot_percent, fat_percent, carb_percent);
//    qDebug() << i << prot_percent << fat_percent << carb_percent;
    if(myname.isEmpty())
    {
        QMessageBox::warning(this, "", tr("Enter your name."),QMessageBox::Ok);
    }
    else
        QDialog::accept();
}

void StartWizard::setDataDir(const QString& val)
{
    datadir = val;
    ((FilePage*)filePage)->setDataDir(val);
}

QString StartWizard::getFileName()
{
    return field("fileName").toString();
}

QString StartWizard::getName()
{
    return field("myName").toString();
}

int StartWizard::getSex()
{
    int sex = field("sex").toInt();
    return sex;
}

QString StartWizard::getDateOfBirth()
{
    return field("dateOfBirth").toDate().toString(Qt::ISODate);
}

double StartWizard::getHeight()
{
    return field("myHeight").toDouble();
}

QString StartWizard::getStartCalcDate()
{
    return field("startDate").toDate().toString(Qt::ISODate);
}

double StartWizard::getWeight()
{
    return field("myWeight").toDouble();
}

int StartWizard::getActivity()
{
    return field("activity").toInt();
}

int StartWizard::getAim()
{
    return field("aim").toInt();
}

QString StartWizard::getExistingFileName()
{
    return field("existingFileName").toString();
}

bool StartWizard::isExistingDict()
{
    return field("isExistingDict").toBool();
}

PersonPage::PersonPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Personal data")); //Персональные данные

    lbName = new QLabel(tr("&Name")); //Имя
    leName = new QLineEdit();
    lbName->setBuddy(leName);
    int w = 92;

    lbSex = new QLabel(tr("Gender"));
    cbSex = new QComboBox();
    cbSex->addItem(tr("Female")); //Женский
    cbSex->addItem(tr("Male")); //Мужской
    cbSex->setEditable(false);
    cbSex->setMinimumWidth(w);
    cbSex->setMaximumWidth(w);
    lbSex->setBuddy(cbSex);

    lbDateOfBirth = new QLabel(tr("&Birthdate"));
    deDateOfBirth = new QDateEdit(QDate(1990,1,1));
    deDateOfBirth->setMinimumWidth(w);
    deDateOfBirth->setMaximumWidth(w);
    lbDateOfBirth->setBuddy(deDateOfBirth);

    lbHeight = new QLabel(tr("&Height, cm"));
    sbHeight = new QSpinBox();
    sbHeight->setMinimum(140);
    sbHeight->setMaximum(240);
    sbHeight->setValue(170);
    sbHeight->setMinimumWidth(w);
    sbHeight->setMaximumWidth(w);
    lbHeight->setBuddy(sbHeight);

    lbStartCalcDate = new QLabel(tr("Nutration start date"));  //Дата начала &учета
    deStartCalcDate = new QDateEdit(QDate::currentDate ());
    deStartCalcDate->setMaximumWidth(92);
    lbStartCalcDate->setBuddy(deStartCalcDate);

    lbWeight = new QLabel(tr("Weight on nutrition start date, kg")); //Вес на дату начала учета, кг:
    sbWeight = new QSpinBox();
    sbWeight->setMinimum(35);
    sbWeight->setMaximum(350);
    sbWeight->setValue(80);
    sbWeight->setMinimumWidth(w);
    sbWeight->setMaximumWidth(w);
    lbWeight->setBuddy(sbWeight);

    lbActive = new QLabel(tr("Activity"));
    cbActive = new QComboBox;
    for(int i=0; i<Def::activityCount(); i++)
        cbActive->addItem(Def::activityName(i),Def::activityData(i));
    cbActive->setEditable(false);
    cbActive->setMinimumWidth(220);
    cbActive->setMaximumWidth(240);
    lbActive->setBuddy(cbActive);

    lbAim = new QLabel(tr("Calorie counting purpose")); //Цель подсчета калорий
    cbAim = new QComboBox;
    for(int i=0; i<Def::aimCount(); i++)
        cbAim->addItem(Def::aimName(i));
    cbAim->setEditable(false);
    cbAim->setMinimumWidth(220);
    cbAim->setMaximumWidth(240);
    lbAim->setBuddy(cbAim);

    QGridLayout *layout = new QGridLayout;
    int row = 0;
    layout->addWidget(lbName, row, 0);
    layout->addWidget(leName, row, 1);
    row++;
    layout->addWidget(lbSex,row,0);
    layout->addWidget(cbSex,row,1);
    row++;
    layout->addWidget(lbDateOfBirth, row, 0);
    layout->addWidget(deDateOfBirth, row, 1);
    row++;
    layout->addWidget(lbHeight, row, 0);
    layout->addWidget(sbHeight, row, 1);
    row++;
    layout->addWidget(lbStartCalcDate, row, 0);
    layout->addWidget(deStartCalcDate, row, 1);
    row++;
    layout->addWidget(lbWeight, row, 0);
    layout->addWidget(sbWeight, row, 1);
    row++;
    layout->addWidget(lbActive, row, 0);
    layout->addWidget(cbActive, row, 1);
    row++;
    layout->addWidget(lbAim, row, 0);
    layout->addWidget(cbAim, row, 1);

    setLayout(layout);

    registerField("myName*", leName);
    registerField("sex", cbSex);
    registerField("dateOfBirth", deDateOfBirth);
    registerField("myHeight", sbHeight);
    registerField("startDate", deStartCalcDate);
    registerField("myWeight", sbWeight);
    registerField("activity", cbActive);
    registerField("aim", cbAim);
}

FilePage::FilePage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Files"));

    QGridLayout *layout1 = new QGridLayout;
    lbFileName = new QLabel(tr("Database file for storing information")); //Файл базы данных для хранения информации:
    leFileName = new QLineEdit();
    lbFileName->setBuddy(leFileName);
    leFileName->setToolTip(tr("Path and file name")); //Путь и имя файла
    tbSelectFile = new QToolButton();
    tbSelectFile->setText("...");
    tbSelectFile->setToolTip(tr("Dialog box for choosing the file")); //Диалоговое окно выбора файла
    layout1->addWidget(lbFileName, 0, 0,1,2);
    layout1->addWidget(leFileName, 1, 0);
    layout1->addWidget(tbSelectFile, 1, 1);

    QGridLayout *layout2 = new QGridLayout;
    cbExistingDict = new QCheckBox(tr("Filling the menu of products and dishes from an existing file")); //Заполнение меню продуктов и блюд из существующего файла:
    leExistingFileName = new QLineEdit();
    leExistingFileName->setToolTip(tr("File from which to borrow the menu of products and dishes")); //Файл, из которого можно позаимствовать меню продуктов и блюд
    tbSelectExistingFile = new QToolButton();
    tbSelectExistingFile->setText("...");
    //Открыть диалоговое окно для выбора файла, из которого следует позаимствовать меню продуктов и блюд
    tbSelectExistingFile->setToolTip(tr("Opens a dialog box for selecting a file from which to borrow the menu of products and dishes"));
    leExistingFileName->setEnabled(false);
    tbSelectExistingFile->setEnabled(false);
    layout2->addWidget(cbExistingDict, 0,0,1,2);
    layout2->addWidget(leExistingFileName, 1, 0);
    layout2->addWidget(tbSelectExistingFile, 1, 1);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(layout1);
    layout->addSpacing(30);
    layout->addLayout(layout2);

    setLayout(layout);

    registerField("fileName*", leFileName);

    connect(tbSelectFile, SIGNAL(clicked()), this, SLOT(selectFile()));
    connect(tbSelectExistingFile, SIGNAL(clicked()), this, SLOT(selectExistingFile()));
    connect(cbExistingDict, SIGNAL(clicked(bool)), leExistingFileName, SLOT(setEnabled(bool)));
    connect(cbExistingDict, SIGNAL(clicked(bool)), tbSelectExistingFile, SLOT(setEnabled(bool)));

    registerField("isExistingDict", cbExistingDict);
    registerField("existingFileName", leExistingFileName);
}

void FilePage::selectFile()
{
    QString fname = QFileDialog::getSaveFileName(this,tr("Create file"),datadir,"SQLite3 (*.sl3)");
    QFileInfo fileinfo;
    fileinfo.setFile(fname);
    if(!fileinfo.exists())
        leFileName->setText(fname);
    else
        QMessageBox::warning(this, "", tr("The file already exists."),QMessageBox::Ok); //Файл  уже существует.
}

void FilePage::selectExistingFile()
{
    QString fname = QFileDialog::getOpenFileName(this,tr("Select file"),datadir, "SQLite3 (*.sl3)");
    if(!fname.isEmpty())
        leExistingFileName->setText(fname);
}
