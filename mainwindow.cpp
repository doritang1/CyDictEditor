#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//1.파일리스트를 작성한다
void MainWindow::on_toolButtonFileSelect_clicked()
{
    //작업할 Directory를 선택한다.
    dirSource.setCurrent(QFileDialog::getExistingDirectory(this, tr("Select Directory"), "../../", QFileDialog::ShowDirsOnly));
    ui->lineEditSourceFile->setText(dirSource.absolutePath());

    //filter를 적용하여 작업 Directory 안의 파일들을 추려낸다.
    QStringList filters;
    filters<<"*.htm";
    listSources = dirSource.entryList(filters);

    //모델을 생성하여 listView와 연결한다.
    modelFiles = new QStringListModel(this);
    modelFiles->setStringList(listSources);
    ui->listViewFiles->setModel(modelFiles);
}

//2.파일하나를 열어 전체 내용을 읽는다.
void MainWindow::on_pushButtonOpen_clicked()
{
        QFile sourceFile;
        sourceFile.setFileName(ui->listViewFiles->currentIndex().data(Qt::DisplayRole).toString());

        if(sourceFile.open(QIODevice::ReadOnly|QIODevice::Text)){

            //한글사용을 위해 fromLocal8Bit함수 사용
            QString text = QString::fromLocal8Bit(sourceFile.readAll());
            ui->plainTextEditContent->setPlainText(text);
        }else{
            //파일 열기에 실패하면 표시하는 메시지
            QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(tr("Warning!!"));
            msgBox->setText(tr("The file can't be opened. Please check if it is in the right folder"));
            msgBox->show();
            return;
        }
        sourceFile.close();
}

//3.각종 에러 가능성을 제거한다.
void MainWindow::on_pushButtonVerify_clicked()
{
    //텍스트박스에 있는 내용을 별도의 버퍼에 담고 텍스트박스 내용은 지운다.
    QString text;
    text = ui->plainTextEditContent->toPlainText();
    //ui->plainTextEditContent->clear();

    QXmlStreamReader verifier(text);
    verifyHtml(verifier);
}

void MainWindow::verifyHtml(QXmlStreamReader &r){

    while(r.readNextStartElement()){
        if(r.name()=="title"){

            ui->lineEditWord->setText(r.tokenString());
        }
    }
}
