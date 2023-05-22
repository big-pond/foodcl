#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
//class QQueue;
class MainWdg;
class StartWizard;
class QueryWdg;
class QueryPWdg;
class QueryPFCWdg;
class QLabel;
class QTranslator;
class ChartWdg;
class MasIdxWdg;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private slots:
    void create();
    int create(const QString& fname);
    void open();
    bool open(const QString& fname);
    void openRecentFile();
    void config();
    void products();
    void person();
    void eatenQuery();
    void dateUseProdQuery();
    void fractPFC();
    void calcMasIdx();
    void help();
    void about();
    void aboutQt();
    void test();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *event);
private:
    Ui::MainWindow *ui;
    QTranslator* translator;
    QTranslator* qt_translator;
    MainWdg* mainwdg;
    QueryWdg* querywdg;
    QueryPWdg* querypwdg;
    QueryPFCWdg* querypfcwdg;
    MasIdxWdg* masidxwdg;

    QDockWidget* chartdocwdg;
    ChartWdg* chartwdg;

    QLabel* lbCopyright;
    QAction* actionChartVisible;

    QStringList recent_file_lst;
    enum {RECENTFILEMAX = 5};
    QAction *separatorRecentFileAct;
    QAction *recentFileActs[RECENTFILEMAX];
    QAction *actionExit;
    void setCurretFile(const QString& fname);
    void updateRecentFileActions();

    void writeSettings();
    void readSettings();
    int addStartRecords(StartWizard *sw);
    bool productsFromExistingDB(const QString& newdbfname, const QString& existdbfname);
    int productsFromSqlFile(const QString& newdbfname, const QString &sqlfname);
};

#endif // MAINWINDOW_H
