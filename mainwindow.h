#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QStringListModel>
#include <QtXml>
#include <QMultiMap>
#include <QMap>
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
    void on_listViewFiles_clicked(const QModelIndex &index);

    void on_listViewWord_clicked(const QModelIndex &index);

    void on_listViewFiles_activated(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QDir dirSource;
    QStringList *listSources;
    QString *stringHtml;
    QString *str;
    QStringListModel *modelFiles;
    QStringListModel *modelTitles;
    QStringList *strlstHtmls;
    QMultiMap<QString, int> mltmapTitles;
    QMap<int, QString> mapDefinitions;
    QXmlStreamReader reader;
    int counterWord;

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
