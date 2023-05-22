#ifndef CONFIGDLG_H
#define CONFIGDLG_H

#include <QDialog>

namespace Ui {
class ConfigDlg;
}

class QTranslator;


class ConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDlg(QTranslator* my_tr, QTranslator* qt_tr, QWidget *parent = 0);
    ~ConfigDlg();
protected:
    void changeEvent(QEvent *event);
public slots:
    void accept();
private slots:
    void selectDataDir();
    void selectQueryDir();
    void changeLanguage(int);
    void setDefaultPFCRatio();
private:
    Ui::ConfigDlg *ui;
    QTranslator* translator;
    QTranslator* qt_translator;
};

#endif // CONFIGDLG_H
