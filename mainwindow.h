#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QStringListModel>
#include <QtXml>
#include <htmlnamedentityresolver.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    HtmlNamedEntityResolver *htmlNamedEntityResolver = NULL;
    ~MainWindow();

private slots:
    void on_toolButtonFileSelect_clicked();
    void on_pushButtonOpen_clicked();
    void on_pushButtonSplit_clicked();

    void on_pushButtonValidate_clicked();

    void on_pushButtonMerge_clicked();

    void on_pushButtonSave_clicked();

private:
    Ui::MainWindow *ui;
    QDir dirSource;
    QStringList *listSources;
    QString *stringHtml;
    QString *str;
    QStringListModel *modelFiles;
    QStringListModel *modelTitles;
    QStringListModel *modelDefinitions;
    QStringList *strlstHtmls;
    QStringList *strlstTitles;
    QStringList *strlstDefinitions;
    QXmlStreamReader reader;

    void readHtmlElement();
    void readHeadElement();
    void readTitleElement();
    void readBodyElement();
    void readPElement();
    void skipUnknownElement();

    bool validateHtml(QString *);
    void splitHtml(QString);
};

#endif // MAINWINDOW_H
