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
    //원래:dirSource.setCurrent(QFileDialog::getExistingDirectory(this, tr("Select Directory"), "../../", QFileDialog::ShowDirsOnly));
    //회사에서:dirSource.setCurrent(QFileDialog::getExistingDirectory(this, tr("Select Directory"), "D:/QtProjects/content/dictionary6/merge", QFileDialog::ShowDirsOnly));
    //집에서:
    dirSource.setCurrent(QFileDialog::getExistingDirectory(this, tr("Select Directory"), "C:/CyberK/QtProjects/dictionary6/merge", QFileDialog::ShowDirsOnly));
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

//3-1: 에러발생 방지코드
void MainWindow::on_pushButtonVerify_clicked()
{


    //텍스트박스에 있는 내용을 별도의 버퍼에 담고 텍스트박스 내용은 지운다.
    QString text;
    text = ui->plainTextEditContent->toPlainText();
    //ui->plainTextEditContent->clear();

    int startPos = 0;
    int offsetPos = 0;
    QStringList tokens;
    tokens << "head" << "title" << "meta" << "link";

    for(int i = 0; i<tokens.size();i++){

        if(tokens.at(i) == "meta" || tokens.at(i) == "link"){

            QString startToken("<"+tokens.at(i));
            QString endToken("</"+tokens.at(i)+">");
            QString startMark("<");
            QStringMatcher matcher;
            matcher.setPattern(startToken);//검색할 문자열(startToken)(ex: "<meta")을 지정함.
            while(1){
                offsetPos = matcher.indexIn(text, startPos);//startToken (ex: "|<meta")을 찾은 첫번째 위치를 기억한다.

                if(offsetPos < 0){
                    startPos = 0;
                    break; //만약 못찾았으면 루프 탈출
                }
                startPos = offsetPos + startToken.length();//startToken의 끝위치(ex: "<meta|"를 검색위치로 새로 지정함

                matcher.setPattern(endToken); //startToken 다음에 오는 endToken (ex: "</meta>")을 찾는다.
                offsetPos = matcher.indexIn(text, startPos);

                if(offsetPos < 0){    //endToken이 찾아지지 않으면
                    matcher.setPattern(startMark);  //첫번째 "<"이 발견되는 위치"|<"를 찾음
                    offsetPos = matcher.indexIn(text, startPos);
                    if(offsetPos < 0) return; //만약 "<"도 발견되지 않으면 함수를 끝냄
                    text = text.insert(offsetPos-1, endToken); //endToken을 집어 넣는다.

                    startPos = offsetPos + endToken.length(); //startPos를 다음 위치로 옮겨놓고
                    matcher.setPattern(startToken); //pattern을 "<"에서 원래의 토큰(ex: "meta")로 다시 바꾼다
                }
            }
        }//meta, link
        else if(tokens.at(i) == "title"){

            QString startToken("<"+tokens.at(i));
            QString endToken("/"+tokens.at(i)+">");
            QString startMark("<");
            QStringMatcher matcher;
            matcher.setPattern(startToken);//검색할 문자열(startToken)(ex: "<title")을 지정함.
            while(1){
                offsetPos = matcher.indexIn(text, startPos);//startToken (ex: "|<title")을 찾은 첫번째 위치를 기억한다.
                if(offsetPos < 0){
                    startPos = 0;
                    break; //만약 못찾았으면 루프 탈출
                }
                startPos = offsetPos + startToken.length();//startToken의 끝위치(ex: "<title|"를 검색위치로 새로 지정함

                matcher.setPattern(endToken); //startToken 다음에 오는 endToken (ex: "/title>")을 찾는다.
                offsetPos = matcher.indexIn(text, startPos);

                if(offsetPos >= 0){    //endToken이 찾아지면
                    if(text.at(offsetPos - 1)== startMark) continue; //endToken(ex: "/title>") 바로 앞이 "<"이면 이상 없으므로 무시
                    text.insert(offsetPos, startMark);  //endToken(ex: "/title>" 바로 앞에 "<"를 삽입한다.

                    startPos = offsetPos + endToken.length(); //startPos를 다음 위치로 옮겨놓고
                    matcher.setPattern(startToken); //pattern을 endToken(ex: "/title>"에서 원래의 토큰(ex: "<title")로 다시 바꾼다
                }
            }
        }//title
        else{

            QString startToken("<"+tokens.at(i));
            QString endToken("</"+tokens.at(i)+">");
            QString startMark("\n");
            QStringMatcher matcher;
            matcher.setPattern(startToken);//검색할 문자열(startToken)(ex: "<head")을 지정함.
            while(1){
                offsetPos = matcher.indexIn(text, startPos);//startToken (ex: "|<head")을 찾은 첫번째 위치를 기억한다.

                if(offsetPos < 0){
                    startPos = 0;
                    break; //만약 못찾았으면 루프 탈출
                }
                startPos = offsetPos + startToken.length();//startToken의 끝위치(ex: "<head|"를 검색위치로 새로 지정함

                matcher.setPattern(endToken); //startToken 다음에 오는 endToken (ex: "</head>")을 찾는다.
                offsetPos = matcher.indexIn(text, startPos);

                if(offsetPos >= 0){    //endToken이 찾아지면
                    if(text.at(offsetPos - 1)== startMark) break; //endToken(ex: "/title>") 바로 앞이 "/n"이면 탈출
                    text.insert(offsetPos, startMark);  //endToken(ex: "/title>") 바로 앞에 "/n"를 삽입한다.

                    startPos = offsetPos + endToken.length(); //startPos를 다음 위치로 옮겨놓고
                    matcher.setPattern(startToken); //pattern을 endToken (ex: "</head>")에서 원래의 토큰(ex: "<head")로 다시 바꾼다

                }

            }
        }//head
    }
    ui->plainTextEditContent->setPlainText(text);
}

//4-1: title과 body문을 추출한다.
void MainWindow::on_pushButtonSplit_clicked()
{
    //텍스트박스에 있는 내용을 별도의 버퍼에 담고 텍스트박스 내용은 지운다.
    QString text;
    text = ui->plainTextEditContent->toPlainText();
    //ui->plainTextEditContent->clear();

    reader.clear();
    reader.addData(text);

    //특수문자 처리를 위한 EntityResolver를 지정해준다.
    if (htmlNamedEntityResolver == NULL) {
        htmlNamedEntityResolver = new HtmlNamedEntityResolver();
    }
    reader.setEntityResolver(htmlNamedEntityResolver);

    //분석 시작
    reader.readNext();
    while(!reader.atEnd()){

        if(reader.isStartElement()){
            if(reader.name() == "html"){
                readHtmlElement();
            }
        }else{
            reader.readNext();
        }
    }

    //에러문 출력
    if(reader.hasError()){
        qDebug()<<reader.errorString();
        qDebug()<<reader.lineNumber();
        qDebug()<<reader.columnNumber();
        qDebug()<<reader.characterOffset();
        qDebug()<<reader.tokenString();
    }
}

//4-2:<html>태그 내부를 읽는다.
void MainWindow::readHtmlElement()
{
    reader.readNext();

    while(!reader.atEnd()){
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if(reader.isStartElement()){
            if(reader.name() == "head"){
                readHeadElement();
            }else if(reader.name() == "body"){
                readBodyElement();
            }else{
                skipUnknownElement();
            }
        }else{
                        //qDebug()<<reader.name()<<"at else code in readHtml before2";
            reader.readNext();
                        //qDebug()<<reader.name()<<"at else code in readHtml after2";
        }
    }
}

//4-3:<head>태그 내부를 읽는다.
void MainWindow::readHeadElement()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == "title") {

                readTitleElement();
            } else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

//4-4<title>태그를 읽는다.
void MainWindow::readTitleElement()
{
    //readElementText()가 불려진 이후에는 EndElement로 이동하므로
    //isEndElement 조건식 이전에 readElementText()가 불려져야 한다.
    ui->lineEditWord->insert(reader.readElementText());

    if (reader.isEndElement()){
        reader.readNext();
    }
}

//4-5: <body>태그를 읽는다.
void MainWindow::readBodyElement()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == "p") {
                readPElement();
            } else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

//4-6: <p>태그를 읽는다.
void MainWindow::readPElement()
{
    QString pStr;

    reader.readNext();
    while(!reader.atEnd()){
        if(reader.tokenType() == QXmlStreamReader::Characters){
            //pStr += reader.text().toString().replace("<","&lt;").replace(">","&gt;"); //글자들을 읽어나가다가(단, <와 >는 &lt;와 &gt;로 남겨둠)
            pStr += reader.text().toString().toHtmlEscaped(); //글자들을 읽어나가다가(단, <와 >는 &lt;와 &gt;로 남겨둠)
        }else if(reader.isStartElement()){
            if(reader.name() == "p"){ // startelement을 만나고 그것이 <p>면
                reader.readNext();
                continue;
            }else if(reader.name() == "font"){ // startelement을 만나고 그것이 <font>면
                reader.readNextStartElement();
                continue;
            }else{ // startelement을 만나고 그것이 <span> 또는 <u>,<sup>이면
                if(reader.attributes().hasAttribute("class")){ // "class" attribute를 가지고 있으면 그대로 표시하고
                    pStr += "<" + reader.name().toString() + " class=\"" + reader.attributes().value("class").toString() + "\">"; //태그를 그대로 붙여준다.
                }else{
                    pStr += "<" + reader.name().toString() + ">"; //"class" attribute가 없으면 태그만 그대로 붙여준다.
                }
            }
        }else if(reader.isEndElement()){ //endelement도 마찬가지다.
            if(reader.name() == "p" || reader.name() == "font" || reader.name() == "body" || reader.name() == "html"){
                reader.readNext();
                continue;
            }else{
                pStr += "</"+ reader.name().toString() + ">";
            }
        }
        reader.readNext();
    }

    //umlaut 있는 타이틀을 뽑아낸다.
    QString titleUml;
    int beginTitlePos = 0;
    int endTitlePos = 0;
    while(1){
        beginTitlePos = indexOf("<b>") + 3;
        if(beginTitlePos <= 3) break;
        endTitlePos = pStr.indexOf("</b>");
        titleUml = pStr.mid(beginTitlePos, endTitlePos-beginTitlePos).remove(QRegExp("<[^>]*>"));
        titleUml = titleUml.remove("|").remove(".");

    }
    ui->lineEditWord->insert(titleUml);


    //전체 body문을 출력한다.
    ui->plainTextEditDefinition->setPlainText(pStr.trimmed());

// 태그를 무시하고 내용만 읽어내는 코드
//    ui->plainTextEditDefinition->appendPlainText(reader.readElementText(QXmlStreamReader::IncludeChildElements));
//    if (reader.isEndElement()){
//        reader.readNext();
//    }
}

//4-7: 알려지지 않은 태그는 건너뛴다.
void MainWindow::skipUnknownElement()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            skipUnknownElement();
        } else {
            reader.readNext();
        }
    }
}
