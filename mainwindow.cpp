//Copyright (C) Yuri Popov 2011-2020
//Contact: Yuri Popov (urmingard@yandex.ru)
//This file is part of application foodcl
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwdg.h"
#include <QCloseEvent>
#include <QSettings>
#include <QMessageBox>
#include <QFileInfo>
#include <QDateTime>
#include <QLabel>
#include <QSqlQuery>
#include <QSplitter>
#include <QProgressDialog>
#include <QTextStream>
#include <QFileDialog>
#include <QDockWidget>
#include <QDesktopServices>
#include <QTranslator>
#include "def.h"
#include "prodwdg.h"
#include "prodwdgdlg.h"
#include "prodtree.h"
#include "startwizard.h"
#include "persondlg.h"
#include "querywdg.h"
#include "querypwdg.h"
#include "querypfcwdg.h"
#include "masidxwdg.h"
#include "chartwdg.h"
#include "configdlg.h"

//#include <QDebug>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QApplication::setOrganizationName("Yuri Popov");
    QApplication::setApplicationName(Def::APP_NAME);
    QApplication::setApplicationDisplayName(tr("Calorie Nutrition")); //Учет калорийности питания
    QApplication::setApplicationVersion("01");
    ui->setupUi(this);
    masidxwdg = 0;
    querywdg = 0;
    querypwdg = 0;
    querypfcwdg = 0;


    translator = new QTranslator(this);
    qt_translator = new QTranslator(this) ;

    mainwdg = new MainWdg();
    setCentralWidget(mainwdg);

    chartdocwdg = new QDockWidget();
    chartdocwdg->setObjectName("ChartDocWdg"); // для QMainWindow::saveState()
    chartdocwdg->setWindowTitle(tr("Charts"));

    chartdocwdg->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    chartdocwdg->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable
                             |QDockWidget::DockWidgetFloatable);
    chartwdg = new ChartWdg();
    chartdocwdg->setWidget(chartwdg);
    addDockWidget(Qt::RightDockWidgetArea, chartdocwdg);

    QSqlDatabase::addDatabase("QSQLITE", Def::CONN_MAIN);

    for (int i = 0; i < RECENTFILEMAX; i++) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
    separatorRecentFileAct = ui->menuFile->addSeparator();
    for (int i = 0; i < RECENTFILEMAX; i++)
        ui->menuFile->addAction(recentFileActs[i]);
    ui->menuFile->addSeparator();
    actionExit = new QAction(tr("E&xit"), this);
    actionExit->setShortcuts(QKeySequence::Quit);
    actionExit->setStatusTip(tr("Exit the application"));
    ui->menuFile->addAction(actionExit);

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionCreate, SIGNAL(triggered()), this, SLOT(create()));
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionPerson, SIGNAL(triggered()), this, SLOT(person()));
    connect(ui->actionProducts, SIGNAL(triggered()), SLOT(products()));
    connect(ui->actionEatenQuery, SIGNAL(triggered()),SLOT(eatenQuery()));
    connect(ui->actionDateUseProd, SIGNAL(triggered()), SLOT(dateUseProdQuery()));
    connect(ui->actionFractPFC, SIGNAL(triggered()), SLOT(fractPFC()));
    connect(ui->actionStatusBarVisible, SIGNAL(triggered(bool)), ui->statusBar, SLOT(setVisible(bool)));
    connect(ui->actionBodyMassIndex, SIGNAL(triggered()), SLOT(calcMasIdx()));
    connect(ui->actionOptions, SIGNAL(triggered()), SLOT(config()));
    connect(ui->actionHelp, SIGNAL(triggered()), SLOT(help()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
//    connect(ui->actionTest, SIGNAL(triggered()), this, SLOT(test()));

    connect(mainwdg, SIGNAL(updateChart()),chartwdg, SLOT(updateCharts()));

    ui->mainToolBar->setVisible(false);
    actionChartVisible = chartdocwdg->toggleViewAction();
    ui->menuView->insertAction(ui->actionStatusBarVisible, actionChartVisible);

    QFileInfo fi(QApplication::applicationFilePath());
    QDateTime dt = fi.lastModified();
    lbCopyright = new QLabel(QString("Copyright © 2011-%1 Yuri Popov").arg(dt.date() .year()));
    ui->statusBar->addWidget(lbCopyright);
    readSettings();
    if(Def::language_id==-1)//Первый запуск
    {
        if (QLocale::system().name()=="ru_RU")
            Def::language_id = 1;
        else
            Def::language_id = 0;
    }
    QLocale::system().name();
    // Загружаем перевод и устанавливаем в приложение
    translator->load(Def::APP_NAME +"_" + Def::languageSuffix(Def::language_id), ".");
    qApp->installTranslator(translator);
    // Загружаем перевод кнопок и устанавливаем в приложение
    qt_translator->load("qtbase_" + Def::languageSuffix(Def::language_id), ".");
    qApp->installTranslator(qt_translator);

    open(Def::filename);
}

MainWindow::~MainWindow()
{
    if(querywdg != 0)  delete querywdg;
    if(querypwdg != 0) delete querypwdg;
    if(querypfcwdg != 0) delete querypfcwdg;
    if(masidxwdg != 0) delete masidxwdg;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
 {
//    int res = QMessageBox::question(this,"",tr("Завершить программу?"),QMessageBox::Yes, QMessageBox::No);
    int res = QMessageBox::Yes;
    if(res==QMessageBox::Yes)
    {
        writeSettings();
        event->accept();
    }
    else
        event->ignore();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        QApplication::setApplicationDisplayName(tr("Calorie Nutrition"));
        this->setWindowTitle(Def::filename);
        actionExit->setText(tr("E&xit"));
        actionExit->setStatusTip(tr("Exit the application"));
        chartdocwdg->setWindowTitle(tr("Charts"));
    }

/*
Динамический перевод и возврат к английскому языку?????
QCoreApplication::removeTranslator ( QTranslator * translationFile )
*/
}

void MainWindow::setCurretFile(const QString &fname)
{
    Def::filename = fname;
    this->setWindowTitle(Def::filename);
    recent_file_lst.removeAll(fname);
    recent_file_lst.prepend(fname);
    while(recent_file_lst.count()>RECENTFILEMAX)
        recent_file_lst.removeLast();
    updateRecentFileActions();
}

void MainWindow::updateRecentFileActions()
{
    int numRecentFiles = qMin(recent_file_lst.count(), (int) RECENTFILEMAX);

    for (int i = 0; i < numRecentFiles; i++) {
        QString text = tr("&%1 %2").arg(i + 1).arg(recent_file_lst[i]);
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(recent_file_lst[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < RECENTFILEMAX; j++)
        recentFileActs[j]->setVisible(false);
    separatorRecentFileAct->setVisible(numRecentFiles > 0);
}

void MainWindow::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("splitter_state", mainwdg->splitter()->saveState());
//    settings.setValue("statusbar",ui->actionToolBarVisible->isChecked());
    settings.setValue("language", Def::language_id);
    settings.endGroup();
    settings.beginGroup("Files");
    settings.setValue("data_dir", Def::datadir);
    settings.setValue("query_dir", Def::querydir);
    settings.setValue("db_file_name", Def::filename);
    settings.setValue("ResentFiles", recent_file_lst);
    settings.endGroup();
    settings.beginGroup("PFC_ratio");
    settings.beginWriteArray("pfc");
    for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
        {
            settings.setArrayIndex(i * 3 + j);
            settings.setValue("percent", Def::pfc_ratio[i][j]);
        }
    settings.endArray();
    settings.setValue("variation", Def::pfc_variation);
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);

    settings.beginGroup("MainWindow");
    bool mainwingeom = settings.contains("geometry");
    if (mainwingeom)
    {
        restoreGeometry(settings.value("geometry").toByteArray());
        restoreState(settings.value("windowState").toByteArray());
    }
    else
        setGeometry(0,25,800, 575);
    mainwdg->splitter()->restoreState(settings.value("splitter_state",mainwdg->splitter()->saveState()).toByteArray());
    Def::language_id = settings.value("language", Def::language_id).toInt();
    settings.endGroup();

    settings.beginGroup("Files");
    Def::datadir = settings.value("data_dir", Def::datadir).toString();
    Def::querydir = settings.value("query_dir", Def::querydir).toString();
    Def::filename = settings.value("db_file_name",Def::filename).toString();
    recent_file_lst = settings.value("ResentFiles").toStringList();
    settings.endGroup();
    settings.beginGroup("PFC_ratio");
    /*int size = */
    settings.beginReadArray("pfc");
    for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
        {
            settings.setArrayIndex(i * 3 + j);
            Def::pfc_ratio[i][j] = settings.value("percent", Def::pfc_ratio[i][j]).toDouble();
        }
    settings.endArray();
    Def::pfc_variation = settings.value("variation", Def::pfc_variation).toDouble();
    settings.endGroup();
}

void MainWindow::create()
{
    StartWizard* sw = new StartWizard();
    sw->setDataDir(Def::datadir);
    if(sw->exec()==QDialog::Accepted)
    {
        QString fname = sw->getFileName();
        create(fname);

        bool res = false;
        if(sw->isExistingDict())
            res = productsFromExistingDB(fname, sw->getExistingFileName());
        if(!res)
            res = productsFromExistingDB(fname, "./default.sl3");
        addStartRecords(sw);
        mainwdg->openDataBase(fname);
        setCurretFile(fname);
    }
    delete sw;
}

int MainWindow::create(const QString &fname)
{
    QString conn("conn_create_db");
    QFile file(fname);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this,"",tr("New database file not created.")); //Файл новой базы не создан.
        return 1;
    }
    {
        QSqlDatabase::addDatabase("QSQLITE", conn);
        QSqlDatabase db = QSqlDatabase::database(conn);
        db.setDatabaseName(fname);
        if (!db.open())
        {
            QMessageBox::information(this,"",tr("New database %1 is not open.").arg(conn)); //Новая база %1 не открыта.
            return 2;
        }
        QSqlQuery query(db);
        QSqlDatabase::database().transaction();
        query.exec("CREATE TABLE days ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "mdate TEXT,"
                   "note TEXT)");
        query.exec("CREATE TABLE meal ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "mtime TEXT,"
                   "weight NUMERIC,"
                   "product_id INTEGER NOT NULL,"
                   "days_id INTEGER NOT NULL,"
                   "FOREIGN KEY (days_id) REFERENCES days(id), "
                   "FOREIGN KEY (product_id) REFERENCES products(id))");
        query.exec("CREATE TABLE person ("
                   "name TEXT,"
                   "dob TEXT,"
                   "height NUMERIC,"
                   "sex INTEGER,"
                   "activity INTEGER,"
                   "prot NUMERIC,"
                   "fat NUMERIC,"
                   "carb NUMERIC,"
                   "variate NUMERIC)");
        query.exec("CREATE TABLE products ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "parent_id INTEGER,"
                   "isgroup INTEGER,"
                   "name TEXT,"
                   "calory NUMERIC, "
                   "protein NUMERIC,"
                   "fat NUMERIC,"
                   "carbohydrate NUMERIC,"
                   "price1kg NUMERIC)");
        query.exec("CREATE TABLE weight ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "weight NUMERIC,"
                   "param1 NUMERIC,"
                   "param2 NUMERIC,"
                   "days_id INTEGER NOT NULL,"
                   "FOREIGN KEY (days_id) REFERENCES days(id))");
        QSqlDatabase::database().commit();
    }
    QSqlDatabase::removeDatabase(conn);
    return 0;
}

bool MainWindow::productsFromExistingDB(const QString &newdbfname, const QString &existdbfname)
{
    int count = 0;
    QString exist_db_conn = "EXIST_CONN";
    QString new_db_conn = "NEW_CONN";
    {
        QSqlDatabase::addDatabase("QSQLITE", exist_db_conn);
        QSqlDatabase::addDatabase("QSQLITE", new_db_conn);
        QSqlDatabase exist_db = QSqlDatabase::database(exist_db_conn);
        QSqlDatabase new_db = QSqlDatabase::database(new_db_conn);
        exist_db.setDatabaseName(existdbfname);
        new_db.setDatabaseName(newdbfname);
        if (exist_db.open()&&new_db.open())
        {
            QSqlQuery queryM(new_db);
            QSqlQuery query1(exist_db);
            int numRec = 0;
            query1.exec("SELECT count(*) FROM products");
            if(query1.next())
                numRec = query1.value(0).toInt();
            QProgressDialog progress(tr("Creating a product table..."), tr("Halt"), 0, numRec, this); //Создаю справочник продуктов... Остановить
            int i = 0;
            query1.exec("SELECT id, parent_id, isgroup, name ,calory, "
                        "protein, fat, carbohydrate, price1kg FROM products");
            QSqlDatabase::database().transaction();
            while(query1.next())
            {
                QString txtq;
                int isgroup = query1.value("isgroup").toInt();
                if(isgroup==1) //группа
                    txtq = QString("INSERT INTO products (id, parent_id, isgroup, name) "
                                   "VALUES (%1, %2, %3, '%4')")
                            .arg(query1.value("id").toInt())
                            .arg(query1.value("parent_id").toInt())
                            .arg(isgroup)
                            .arg(query1.value("name").toString());
                else
                    txtq = QString("INSERT INTO products "
                                   "(id, parent_id, isgroup, name ,calory, "
                                   "protein, fat, carbohydrate, price1kg) "
                                   "VALUES (%1, %2, %3, '%4', %5, %6, %7, %8, %9)")
                            .arg(query1.value("id").toInt())
                            .arg(query1.value("parent_id").toInt())
                            .arg(isgroup)
                            .arg(query1.value("name").toString())
                            .arg(query1.value("calory").toDouble())
                            .arg(query1.value("protein").toDouble())
                            .arg(query1.value("fat").toDouble())
                            .arg(query1.value("carbohydrate").toDouble())
                            .arg(query1.value("price1kg").toDouble());
                if(queryM.exec(txtq))
                    count++;
                progress.setValue(i);
                QApplication::processEvents();
                i++;
                if (progress.wasCanceled())
                    break;
            }
            QSqlDatabase::database().commit();
            progress.setValue(numRec);
        }
    }
    QSqlDatabase::removeDatabase(exist_db_conn);
    QSqlDatabase::removeDatabase(new_db_conn);
    return (count>0);
}

int MainWindow::productsFromSqlFile(const QString &newdbfname, const QString &sqlfname)
{
    int countStr = 0;
    QString new_db_conn = "NEW_CONN";
    QString str;
    QFile file(sqlfname);
    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly))
        {
            QSqlDatabase::addDatabase("QSQLITE", new_db_conn);
            QSqlDatabase new_db = QSqlDatabase::database(new_db_conn);
            new_db.setDatabaseName(newdbfname);
            if (new_db.open())
            {
                QSqlQuery query(new_db);
                QTextStream ts(&file);
                while(!ts.atEnd())
                {
                    str = ts.readLine();
                    countStr++;
                }
                ts.seek(0);
                ts.setCodec("UTF-8");
                QProgressDialog progress("Сopying the product table", "Halt", 0, countStr, this);
                int i = 0;
                QSqlDatabase::database().transaction();
                while(!ts.atEnd())
                {
                    str = ts.readLine();
                    query.exec(str);
                    progress.setValue(i);
                    QApplication::processEvents();
                    i++;
                }
                QSqlDatabase::database().commit();
                progress.setValue(countStr);
            }
            file.close();
        }
        QSqlDatabase::removeDatabase(new_db_conn);
    }
    return countStr;
}

int MainWindow::addStartRecords(StartWizard *sw)
{
    QString conn("conn_addstartrec_db");
    {
        QSqlDatabase::addDatabase("QSQLITE", conn);
        QSqlDatabase db = QSqlDatabase::database(conn);
        db.setDatabaseName(sw->getFileName());
        if (!db.open())
        {
            QMessageBox::information(this,"",tr("The %1 database is not open for adding start records.").arg(conn));//База %1 не открыта для добавления стартовых записей.
            return 2;
        }
        QSqlQuery query(db);
        int id = 1;
        QSqlDatabase::database().transaction();
        query.exec(QString("INSERT INTO person VALUES('%1','%2',%3,%4,%5,%6,%7,%8,%9)")
                   .arg(sw->getName()).arg(sw->getDateOfBirth())
                   .arg(sw->getHeight()).arg(sw->getSex()).arg(sw->getActivity())
                   .arg(sw->getProtPercent()).arg(sw->getFatPercent()).arg(sw->getCarbPercent()).arg(Def::pfc_variation,0,'f',1));
        query.exec(QString("INSERT INTO days (id, mdate, note) VALUES (%1, '%2', '%3')")
                   .arg(id).arg(sw->getStartCalcDate()).arg(tr("Start of accounting")));   //Начало учета
        query.exec(QString("INSERT INTO weight (weight, days_id) VALUES ('%1', %2)")
                   .arg(sw->getWeight()).arg(id));
        QSqlDatabase::database().commit();
    }
    QSqlDatabase::removeDatabase(conn);
    return 0;
}

void MainWindow::open()
{
    QString fname = QFileDialog::getOpenFileName(this,tr("Open file"),Def::datadir,"SQLite3 (*.sl3)"); // Открыть файл
    open(fname);
}

bool MainWindow::open(const QString &fname)
{
    bool result = false;
     if(QFileInfo::exists(fname))
     {
        mainwdg->openDataBase(fname);
        setCurretFile(fname);
     }
     else
         QString s(tr("File not found."));  //Файл не найден.
     return result;
}
void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        open(action->data().toString());
}

void MainWindow::config()
{
    ConfigDlg* dlg = new ConfigDlg(translator, qt_translator);
    if(dlg->exec()==QDialog::Accepted)
    {
        mainwdg->setEditDayButtonVisible(Def::showbuttonstabdays);
    }
    delete dlg;
}

void MainWindow::products()
{
    ProdwdgDlg* dlg = new ProdwdgDlg();
    dlg->readData();
    dlg->exec();

//    ProdWdg* pwdg = new ProdWdg();
//    pwdg->setAttribute(Qt::WA_DeleteOnClose);
//    pwdg->getProdTree()->readData();
//    pwdg->show();
}

void MainWindow::person()
{
    PersonDlg* pdlg = new PersonDlg();
    pdlg->selectData();
    pdlg->exec();
    delete pdlg;
}

void MainWindow::eatenQuery()
{
    if(querywdg==0)
    {
        querywdg = new QueryWdg();
    }
    querywdg->show();
}

void MainWindow::dateUseProdQuery()
{
    if(querypwdg==0)
    {
        querypwdg = new QueryPWdg();
    }
    querypwdg->show();
}

void MainWindow::fractPFC()
{
    if(querypfcwdg==0)
    {
        querypfcwdg = new QueryPFCWdg();
    }
    querypfcwdg->show();

}

void MainWindow::calcMasIdx()
{
    if(masidxwdg==0)
        masidxwdg = new MasIdxWdg();
    if (masidxwdg!=0)
        masidxwdg->show();
}

void MainWindow::help()
{
//    if(appsettings.hlpurl.contains("http:"))
//        QDesktopServices::openUrl(QUrl::fromUserInput(appsettings.hlpurl));
//    else
        QDesktopServices::openUrl(QUrl::fromLocalFile("./help/index.html"));
}

void MainWindow::about()
{
    QFileInfo fi(QApplication::applicationFilePath());
    QDateTime dt = fi.lastModified();
    QMessageBox::about(this, tr("About"),
        tr("<h2 align=\"center\"><font color=\"#800080\">%1</font></h2>"
           "<p align=\"center\">Version %2</p>"
           "<p align=\"center\">Build: %3</p>"
           "<p align=\"center\">%4</p>"
           "<p align=\"center\">Application website  <a href=\"%5\">%5</a></p>"
           "<p align=\"justify\">This program is free software. "
           "You can distribute it and / or modify it by following the terms "
           "Open source License Agreement (GNU General Public License) version 3,  "
           "published by the Free Software Foundation, or (at your choice) any subsequent."
           "<br>This program is distributed in the hope that it will be useful, "
           "but WITHOUT any WARRANTY or FITNESS for a PARTICULAR PURPOSE."
           "<br>To develop the program, we used the QT Creator IDE with the MinGW C++ compiler and the Qt library.</p>"
           "<p align=\"justify\"><font color=\"#005000\">You can support further development of the program by making a donation "
           "through the  payment system Yandex.Money. Account number </font><font color=\"#500000\">410011981232738.</font></p>"
           ).arg(QApplication::applicationDisplayName())
            .arg(QApplication::applicationVersion())
            .arg(dt.toString("dd.MM.yyyy hh:mm"))
            .arg(lbCopyright->text())
            .arg("https://www.foodcl.ucoz.net"));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::test()
{
//    if(chartwdg==0)  chartwdg = new ChartWdg();
//    chartwdg->show();
//    chartwdg->updateCharts();
}

