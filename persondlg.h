#ifndef PERSONDLG_H
#define PERSONDLG_H

#include <QDialog>

class QLineEdit;
class QDateEdit;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QLabel;
class QPushButton;

class PersonDlg : public QDialog
{
    Q_OBJECT
private slots:
    void updateData();
    void setEnabledPBtUpdate();
public slots:
    virtual void accept ();
public:
    explicit PersonDlg(QWidget *parent = 0);
    void selectData();
protected:
    virtual void closeEvent(QCloseEvent * event);
private:
    QString name;
    QLineEdit* leName;
    QDateEdit* deDOB;
    QSpinBox* sbHeight;
    QSpinBox* sbProt;
    QSpinBox* sbFat;
    QSpinBox* sbCarb;
    QDoubleSpinBox* sbVariation;
    QComboBox* cbSex;
    QComboBox* cbActive;
    QLabel* lbNormWeight;
    QLabel* lbDate;
    QLabel* lbWeight;
    QLabel* lbIndex;

    QPushButton* pbUpdate;
    QPushButton* pbClose;

    void writeSettings();
    void readSettings();
};

#endif // PERSONDLG_H
