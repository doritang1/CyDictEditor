#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QStringListModel>
#include <QtXml>
#include <QMultiMap>
#include <QMap>
#include <htmlnamedentityresolver.h>

#include <stdint.h>

namespace Ui {
class MainWindow;
}

//파일포인터를 저장하기 위한 구조체
struct _position
{
    uint32_t contentBegin;
    int contentLength;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    HtmlNamedEntityResolver *htmlNamedEntityResolver = NULL;
    ~MainWindow();

private slots:
    void on_toolButtonFileSelect_clicked();
    void on_pushButtonSplit_clicked();
    void on_pushButtonValidate_clicked();
    void on_pushButtonMerge_clicked();
    void on_pushButtonSave_clicked();
    void on_listViewFiles_clicked(const QModelIndex &index);
    void on_listViewWordFromMap_clicked(const QModelIndex &index);
    void on_pushButtonCreateDict_clicked();
    void on_listViewWordFromFile_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QDir dirSource;
    QStringList *listSources;
    QString *stringHtml;
    QString *str;
    QStringListModel *modelFiles;
    QStringListModel *modelTitles;
    QStringListModel *modelWords;
    QStringList *strlstHtmls;
    //파일에 저장하기 전에 임시로 쓰는 자료구조
    QMultiMap<QString, int> mltmapTitles;
    QMap<int, QString> mapDefinitions;
    //파일에 저장된 타이틀을 불러올 때 쓰는 자료구조
    QMultiMap<QString, _position> mltmapWords;
    QString strFileName;

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

    void createDict(QString &dictionaryName);
    void loadDict(QString &strFilePath);
};

#endif // MAINWINDOW_H
