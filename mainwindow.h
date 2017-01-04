#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QStringListModel>
#include <QtXml>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_toolButtonFileSelect_clicked();
    void on_pushButtonOpen_clicked();
    void on_pushButtonVerify_clicked();

private:
    Ui::MainWindow *ui;
    QDir dirSource;
    QStringList listSources;
    QStringListModel *modelFiles;

    void verifyHtml(QXmlStreamReader &r);
};

#endif // MAINWINDOW_H
