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
    //원래
    //:dirSource.setCurrent(QFileDialog::getExistingDirectory(this, tr("Select Directory"), "../../", QFileDialog::ShowDirsOnly));
    //회사에서:
    //dirSource.setCurrent(QFileDialog::getExistingDirectory(this, tr("Select Directory"), "D:/QtProjects/content/dictionary6/merge", QFileDialog::ShowDirsOnly));
    //집에서:
    dirSource.setCurrent(QFileDialog::getExistingDirectory(this, tr("Select Directory"), "C:/CyberK/QtProjects/dictionary6/merge", QFileDialog::ShowDirsOnly));
    ui->lineEditSourceFile->setText(dirSource.absolutePath());

    //filter를 적용하여 작업 Directory 안의 파일들을 추려낸다.
    QStringList filters;
    filters<<"*.htm";
    listSources = new QStringList(dirSource.entryList(filters));

    //모델을 생성하여 listView와 연결한다.
    modelFiles = new QStringListModel(this);
    modelFiles->setStringList(*listSources);
    ui->listViewFiles->setModel(modelFiles);
    ui->listViewFiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

//2.파일을 하나 열어 그 내용을 읽는다.
void MainWindow::on_pushButtonOpen_clicked()
{
    QFile sourceFile;
    str = new QString();
    sourceFile.setFileName(ui->listViewFiles->currentIndex().data(Qt::DisplayRole).toString());
    //sourceFile.setFileName(index.data(Qt::DisplayRole).toString());
    if(sourceFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        //한글사용을 위해 fromLocal8Bit함수 사용
        *str = QString::fromLocal8Bit(sourceFile.readAll()); //파일을 읽어
        ui->plainTextEditContent->setPlainText(*str); //보여준다.

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

void MainWindow::on_listViewFiles_clicked(const QModelIndex &index)
{
    QFile sourceFile;
    str = new QString();
    //sourceFile.setFileName(ui->listViewFiles->currentIndex().data(Qt::DisplayRole).toString());
    sourceFile.setFileName(index.data(Qt::DisplayRole).toString());
    if(sourceFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        //한글사용을 위해 fromLocal8Bit함수 사용
        *str = QString::fromLocal8Bit(sourceFile.readAll()); //파일을 읽어
        ui->plainTextEditContent->setPlainText(*str); //보여준다.

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

void MainWindow::on_listViewFiles_activated(const QModelIndex &index)
{
    QFile sourceFile;
    str = new QString();
    //sourceFile.setFileName(ui->listViewFiles->currentIndex().data(Qt::DisplayRole).toString());
    sourceFile.setFileName(index.data(Qt::DisplayRole).toString());
    if(sourceFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        //한글사용을 위해 fromLocal8Bit함수 사용
        *str = QString::fromLocal8Bit(sourceFile.readAll()); //파일을 읽어
        ui->plainTextEditContent->setPlainText(*str); //보여준다.

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

//3.한 개의 파일을 점검한다.(validate)
void MainWindow::on_pushButtonValidate_clicked()
{
    if(validateHtml(str)){ //이상 없는지 점검하고
        ui->plainTextEditContent->setPlainText(*str);
    }
}

//3-1: 에러발생 방지코드
bool MainWindow::validateHtml(QString *strHtml)
{
    int startPos = 0;
    int offsetPos = 0;
    QStringList tokens;
    tokens << "head" << "title" << "meta" << "link" << "span" << "br" << "Object" << "param" << "p";

    for(int i = 0; i<tokens.size();i++){

        if(tokens.at(i) == "meta" || tokens.at(i) == "link"){
            QString startToken("<"+tokens.at(i));
            QString endToken("</"+tokens.at(i)+">");
            QString startMark("<");
            QStringMatcher matcher;
            matcher.setPattern(startToken);//검색할 문자열(startToken)(ex: "<meta")을 지정함.
            while(1){

                offsetPos = matcher.indexIn(*strHtml, startPos);//startToken (ex: "|<meta")을 찾은 첫번째 위치를 기억한다.
                if(offsetPos < 0){
                    startPos = 0;
                    break; //만약 못찾았으면 루프 탈출
                }
                startPos = offsetPos + startToken.length();//startToken의 끝위치(ex: "<meta|"를 검색위치로 새로 지정함

                matcher.setPattern(endToken); //startToken 다음에 오는 endToken (ex: "</meta>")을 찾는다.
                offsetPos = matcher.indexIn(*strHtml, startPos);

                if(offsetPos < 0){    //endToken이 찾아지지 않으면
                    matcher.setPattern(startMark);  //첫번째 "<"이 발견되는 위치"|<"를 찾음
                    offsetPos = matcher.indexIn(*strHtml, startPos);
                    if(offsetPos < 0) return true; //만약 "<"도 발견되지 않으면 함수를 끝냄
                    *strHtml = strHtml->insert(offsetPos-1, endToken); //endToken을 집어 넣는다.

                    startPos = offsetPos + endToken.length(); //startPos를 다음 위치로 옮겨놓고
                    matcher.setPattern(startToken); //pattern을 "<"에서 원래의 토큰(ex: "meta")로 다시 바꾼다
                }
                startPos = offsetPos + endToken.length(); //startPos를 다음 위치로 옮겨놓고
                matcher.setPattern(startToken); //pattern을 "<"에서 원래의 토큰(ex: "meta")로 다시 바꾼다
            }
        }//meta, link
        else if(tokens.at(i) == "title"){
            QString startToken("<"+tokens.at(i));
            QString endToken("/"+tokens.at(i)+">");
            QString startMark("<");
            QStringMatcher matcher;
            matcher.setPattern(startToken);//검색할 문자열(startToken)(ex: "<title")을 지정함.
            while(1){
                offsetPos = matcher.indexIn(*strHtml, startPos);//startToken (ex: "|<title")을 찾은 첫번째 위치를 기억한다.
                if(offsetPos < 0){
                    startPos = 0;
                    break; //만약 못찾았으면 루프 탈출
                }
                startPos = offsetPos + startToken.length();//startToken의 끝위치(ex: "<title|"를 검색위치로 새로 지정함
                matcher.setPattern(endToken); //startToken 다음에 오는 endToken (ex: "/title>")을 찾는다.
                offsetPos = matcher.indexIn(*strHtml, startPos);
                if(offsetPos >= 0){    //endToken이 찾아지면
                    if(strHtml->at(offsetPos - 1)== startMark) continue; //endToken(ex: "/title>") 바로 앞이 "<"이면 이상 없으므로 무시
                    strHtml->insert(offsetPos, startMark);  //endToken(ex: "/title>" 바로 앞에 "<"를 삽입한다.
                    startPos = offsetPos + endToken.length(); //startPos를 다음 위치로 옮겨놓고
                    matcher.setPattern(startToken); //pattern을 endToken(ex: "/title>"에서 원래의 토큰(ex: "<title")로 다시 바꾼다
                }
            }
        }//title
        else if(tokens.at(i) == "head"){
            QString startToken("<"+tokens.at(i));
            QString endToken("</"+tokens.at(i)+">");
            QString startMark("\n");
            QStringMatcher matcher;
            matcher.setPattern(startToken);//검색할 문자열(startToken)(ex: "<head")을 지정함.
            while(1){
                offsetPos = matcher.indexIn(*strHtml, startPos);//startToken (ex: "|<head")을 찾은 첫번째 위치를 기억한다.

                if(offsetPos < 0){
                    startPos = 0;
                    break; //만약 못찾았으면 루프 탈출
                }
                startPos = offsetPos + startToken.length();//startToken의 끝위치(ex: "<head|"를 검색위치로 새로 지정함

                matcher.setPattern(endToken); //startToken 다음에 오는 endToken (ex: "</head>")을 찾는다.
                offsetPos = matcher.indexIn(*strHtml, startPos);

                if(offsetPos >= 0){    //endToken이 찾아지면
                    if(strHtml->at(offsetPos - 1)== startMark) break; //endToken(ex: "</head>") 바로 앞이 "\n"이면 탈출
                    strHtml->insert(offsetPos, startMark);  //endToken(ex: "</head>") 바로 앞에 "\n"를 삽입한다.

                    startPos = offsetPos + endToken.length(); //startPos를 다음 위치로 옮겨놓고
                    matcher.setPattern(startToken); //pattern을 endToken (ex: "</head>")에서 원래의 토큰(ex: "<head")로 다시 바꾼다
                }//if(offsetPos)
            }//while(1)
        }//head
        else if(tokens.at(i) == "span"){
            *strHtml = strHtml->replace("<SPAN", "<span"); //대문자를 소문자로
            *strHtml = strHtml->replace("</SPAN", "</span");

            //span태그의 class 애트리뷰트에서 따옴표가 없음
            int s = 0;
            int e = 0;
            s = strHtml->indexOf(QRegExp("class=[^\"].*[^\"]>"));
            if(s>0){
                e = strHtml->indexOf(">",s);
                *strHtml = strHtml->insert(e,"\"");
                *strHtml = strHtml->insert(s+6,"\"");
            }

        }//SPAN이 대문자 or 애트리뷰트에 따옴표 없음
        else if(tokens.at(i) == "br"){
            *strHtml = strHtml->remove("<br>"); //<br> 제거
        }//br
        else if(tokens.at(i) == "Object" || tokens.at(i) == "param"){
            *strHtml = strHtml->remove(QRegExp("<Object[^>]*>"));
            *strHtml = strHtml->remove(QRegExp("<param[^>]*>"));
        }//Object, param
        else if(tokens.at(i) == "p" || tokens.at(i) == "b"){
            //qDebug()<<*strHtml;
            *strHtml = strHtml->replace("<P>", "<p>"); //P가 대문자
            *strHtml = strHtml->replace("</P>", "</p>");
            if(strHtml->contains("<p>&nbsp;")){ //"<p>&nbsp;"가 있으면

                int s = 0;
                int e = 0;
                QString strBefore;
                QString strAfter;
                while(1){
                    s = strHtml->indexOf("<p>&nbsp;", s);
                    if(s>0){  //<p>&nbsp;를 찾고
                        e = strHtml->indexOf("\n",s);
                        if(e>0){ //다시 \n를 찾아 있으면
                            if(strHtml->mid(e-4, 4)=="</p>"){ //그 앞에 </p>가 있는지 검사
                                break;
                            }else{ //앞에 </p>가 없으면
                                strBefore = strHtml->mid(s+3, e-(s+3));
                                strAfter = strBefore;
                                strAfter = strAfter.append("</p>");//뒤에 나오는 "\n"앞에 </p>를 삽입한다.
                                *strHtml = strHtml->replace(strBefore, strAfter);
                                s += 3; //앞의 <p>뒤에서 다시 시작
                                e = 0;
                            }
                        }
                    }else{ //<p>&nbsp;가 없으면
                        break;
                    }
                }
            }
            *strHtml = strHtml->replace("<B>", "<b>"); //B가 대문자
            *strHtml = strHtml->replace("</B>", "</b>");

            //<p>와 </p>사이에 "\n"(줄바꿈)이 있다. </p>가 없다.
            int s = 0;
            int e = 0;
            QString strBefore;
            QString strAfter;
            while(1){
                s = strHtml->indexOf("<p>",s);
                if(s>0){ //<p>를 찾았으면
                    e = strHtml->indexOf("</p>",s);
                    if(e>0){ //다시 </p>를 찾고
                        strBefore = strHtml->mid(s+3,e-(s+3)); //p태그 사이에 있는 문장만 뽑아낸다.
                        strAfter = strBefore;
                        if(strBefore.contains("\n")){ // "\n"이 들어있으면
                            strAfter = strAfter.replace("\n", " ").toUtf8(); // "\n"을 공백(" ")으로 대체한다.
                        }
                        *strHtml = strHtml->replace(strBefore, strAfter);
                        s += 3; //앞의 <p>뒤에서 다시 시작.
                        e = 0;
                        //continue;
                    }else{  //<p>는 있는데 </p>가 없다.
                        e = strHtml->indexOf("<p>", s+3); //혹시 뒤에 다른 <p>가 나오는지 봐서
                        if(e>0){
                            strBefore = strHtml->mid(s+3,e-(s+3)); //<p>와 <p>태그 사이에 있는 문장을 뽑아낸다.
                            strAfter = strBefore;
                            strAfter = strAfter.append("</p>"); //뒤에 나오는 <p>앞에 </p>를 삽입한다.
                            *strHtml = strHtml->replace(strBefore, strAfter);
                            s += 3; //앞의 <p>뒤에서 다시 시작
                            e = 0;
                        }else{ //만약 다른 <p>도 찾을 수 없다면
                            e = strHtml->indexOf("\n", s+3); // 줄의 끝("\n")을 찾아
                            strBefore = strHtml->mid(s+3, e-(s+3));
                            strAfter = strBefore;
                            strAfter = strAfter.append("</p>");//뒤에 나오는 "\n"앞에 </p>를 삽입한다.
                            *strHtml = strHtml->replace(strBefore, strAfter);
                            s += 3; //앞의 <p>뒤에서 다시 시작
                            e = 0;
                        }
                        //continue;
                    }
                }else{ //더이상 <p>를 못찾으면 탈출
                    break;
                }
            }

        }//p: <&nbsp || <P> || <B>
        else {
            QString startToken("<"+tokens.at(i));
            QString endToken("</"+tokens.at(i)+">");
            QString startMark("\n");
            QStringMatcher matcher;
            matcher.setPattern(startToken);//검색할 문자열(startToken)(ex: "<head")을 지정함.
            while(1){
                offsetPos = matcher.indexIn(*strHtml, startPos);//startToken (ex: "|<head")을 찾은 첫번째 위치를 기억한다.

                if(offsetPos < 0){
                    startPos = 0;
                    break; //만약 못찾았으면 루프 탈출
                }
                startPos = offsetPos + startToken.length();//startToken의 끝위치(ex: "<head|"를 검색위치로 새로 지정함

                matcher.setPattern(endToken); //startToken 다음에 오는 endToken (ex: "</head>")을 찾는다.
                offsetPos = matcher.indexIn(*strHtml, startPos);

                if(offsetPos >= 0){    //endToken이 찾아지면
                    if(strHtml->at(offsetPos - 1)== startMark) break; //endToken(ex: "</head>") 바로 앞이 "\n"이면 탈출
                    strHtml->insert(offsetPos, startMark);  //endToken(ex: "</head>") 바로 앞에 "\n"를 삽입한다.

                    startPos = offsetPos + endToken.length(); //startPos를 다음 위치로 옮겨놓고
                    matcher.setPattern(startToken); //pattern을 endToken (ex: "</head>")에서 원래의 토큰(ex: "<head")로 다시 바꾼다
                }//if(offsetPos)
            }//while(1)
        }//else
    }//for(int i = 0; i<tokens.size();i++)
    return true;
}

//4: 한 개의 파일을 수정후 저장
void MainWindow::on_pushButtonSave_clicked()
{
    QFile targetFile;
    targetFile.setFileName(ui->listViewFiles->currentIndex().data(Qt::DisplayRole).toString());
    if(targetFile.open(QIODevice::WriteOnly|QIODevice::Text)){
        //한글사용을 위해 fromLocal8Bit함수 사용
        targetFile.write(ui->plainTextEditContent->toPlainText().toLocal8Bit());
        ui->plainTextEditContent->clear();
    }else{
        //파일 열기에 실패하면 표시하는 메시지
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(tr("Warning!!"));
        msgBox->setText(tr("The file can't be opened. Please check if it is in the right folder"));
        msgBox->show();
        return;
    }
       targetFile.close();
}

//5: 모든 파일내용을 한개의 리스트로 옮긴다.(merge)
void MainWindow::on_pushButtonMerge_clicked()
{
    ui->plainTextEditContent->clear();
    ui->plainTextEditDefinition->clear();

    QFile sourceFile;
    QStringListIterator itorFiles(*listSources); //파일의 리스트 작성
    stringHtml = new QString();
    strlstHtmls = new QStringList();
    while(itorFiles.hasNext()){ //리스트를 순환하면서
         sourceFile.setFileName(itorFiles.next());
       if(sourceFile.open(QIODevice::ReadOnly|QIODevice::Text)){
           //한글사용을 위해 fromLocal8Bit함수 사용
           *stringHtml = QString::fromLocal8Bit(sourceFile.readAll()); //파일을 읽어
           if(validateHtml(stringHtml)){ //이상 없는지 점검하고

               strlstHtmls->append(*stringHtml); //StringList에 담는다.
           }
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
    QMessageBox::information(this, "Succeeded!!!", QString("Total %1 files are successfully merged.").arg(strlstHtmls->length()), "Cofirm");
    ui->plainTextEditContent->setPlainText(QString("Total %1 files are successfully merged.").arg(strlstHtmls->length()));
    ui->plainTextEditDefinition->setPlainText(QString("Total %1 files are successfully merged.").arg(strlstHtmls->length()));
}

//6: title과 body문을 추출한다.
void MainWindow::on_pushButtonSplit_clicked()
{
    ui->plainTextEditContent->clear();
    ui->plainTextEditDefinition->clear();
    ui->listViewWord->clearSelection();
    //html들이 담겨있는 리스트 strlstHtmls를 순환하면서 QXmlReader로 분석
    QStringListIterator itorHtmls(*strlstHtmls);
    counterWord = 0;
    while(itorHtmls.hasNext()){
        ++counterWord;
        splitHtml(itorHtmls.next());
    }

    //모델을 생성하여 listView와 연결한다.
    modelTitles = new QStringListModel(this);
    modelTitles->setStringList(mltmapTitles.keys());
    ui->listViewWord->setModel(modelTitles);

    QMessageBox::information(this, "Succeeded!!!", QString("Total %1 files are successfully processed.").arg(strlstHtmls->length()), "Cofirm");
    ui->plainTextEditContent->setPlainText(QString("Total %1 files are successfully processed.").arg(strlstHtmls->length()));
}

void MainWindow::splitHtml(QString text)
{
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
        }//if(reader.isStartElement())
    }//while(!reader.atEnd()

    //에러문 출력
    if(reader.hasError()){
//        QMessageBox::information(this, "Error!!!",
//                                 QString("%1\nLine: %2\nColumn: %3\nCharacter at: %4\nToken: %5\nHtml: ...%6...")
//                                        .arg(reader.errorString())
//                                        .arg(reader.lineNumber())
//                                        .arg(reader.columnNumber())
//                                        .arg(reader.characterOffset())
//                                        .arg(reader.tokenString())
//                                        .arg(text.mid(reader.characterOffset()-10, 20)),
//                                 "OK");
        int s = text.indexOf("<title>");
        int e = text.indexOf("</title>");
        QString str = text.mid(s + 7, e-(s+7));
        ui->plainTextEditDefinition->appendPlainText(QString("Title: %1\nError: %2\nPart: %3\n").arg(str).arg(reader.errorString()).arg(text.mid(reader.characterOffset()-30, 40)));
    }//if(reader.hasError())
}

//6-1:<html>태그 내부를 읽는다.
void MainWindow::readHtmlElement()
{
    reader.readNext();
    while(!reader.atEnd()){
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }//if (reader.isEndElement())

        if(reader.isStartElement()){
            if(reader.name() == "head"){
                readHeadElement();
            }else if(reader.name() == "body"){
                readBodyElement();
            }else{
                skipUnknownElement();
            }//if(reader.name() == "head"
        }else{
            reader.readNext();
        }//if(reader.isStartElement())
    }//while(!reader.atEnd())
}

//6-2:<head>태그 내부를 읽는다.
void MainWindow::readHeadElement()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }//if (reader.isEndElement())

        if (reader.isStartElement()) {
            if (reader.name() == "title") {
                readTitleElement();
            } else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }//if (reader.isStartElement())
    }//while (!reader.atEnd())
}

//6-3<title>태그를 읽는다.
void MainWindow::readTitleElement()
{
    //readElementText()가 불려진 이후에는 EndElement로 이동하므로
    //isEndElement 조건식 이전에 readElementText()가 불려져야 한다.
    QString title = reader.readElementText();
    if(title.length()>0){ //빈문자열("")은 건너뛴다.
        mltmapTitles.insert(title, counterWord);
    }

    if (reader.isEndElement()){
        reader.readNext();
    }
}

//6-4: <body>태그를 읽는다.
void MainWindow::readBodyElement()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }//if (reader.isEndElement())

        if (reader.isStartElement()) {
            if (reader.name() == "p") {
                readPElement();
            } else {
                skipUnknownElement();
            }//if (reader.name() == "p")
        } else {
            reader.readNext();
        }//if (reader.isStartElement())
    }
}

//6-5: <p>태그를 읽는다.
void MainWindow::readPElement()
{
    QString pStr;

    reader.readNext();
    while(!reader.atEnd()){
        if(reader.tokenType() == QXmlStreamReader::Characters){
            //글자들을 읽어나가다가(단, <와 >는 &lt;와 &gt;로 남겨둠)
            pStr += reader.text().toString().toHtmlEscaped();
        }else if(reader.isStartElement()){
            if(reader.name() == "p"){ // startelement을 만나고 그것이 <p>면
                reader.readNext();
                continue;
            }else if(reader.name() == "font"){ // startelement을 만나고 그것이 <font>면
                reader.readNextStartElement();
                continue;
            }else{ // startElement을 만나고 그것이 <span> 또는 <u>,<sup>이면
                // "class" attribute를 가지고 있으면 그대로 표시하고
                if(reader.attributes().hasAttribute("class")){
                    pStr += "<" + reader.name().toString()
                            + " class=\""
                            + reader.attributes().value("class").toString()
                            + "\">"; //태그를 그대로 붙여준다.
                }else{
                    //"class" attribute가 없으면 태그만 그대로 붙여준다.
                    pStr += "<" + reader.name().toString() + ">";
                }
            }//if(reader.name() == ?)
        }else if(reader.isEndElement()){ //endelement도 마찬가지다.
            if(reader.name() == "p" || reader.name() == "font" || reader.name() == "body" || reader.name() == "html"){
                reader.readNext();
                continue;
            }else{
                pStr += "</"+ reader.name().toString() + ">";
            }//if(reader.name() == ?)
        }//else if(reader.isEndElement())
        reader.readNext();
    }//while(!reader.atEnd())

    //타이틀 추출하여 움라우트를 가지고 있으면
    QString titleUml;
    int beginTitlePos = 0;
    int endTitlePos = 0;
    while(1){
        beginTitlePos = pStr.indexOf("<b>", beginTitlePos);
        if(beginTitlePos < 0) break;
        endTitlePos = pStr.indexOf("</b>", beginTitlePos);
        titleUml += pStr.mid(beginTitlePos, endTitlePos-beginTitlePos).remove(QRegExp("<[^>]*>"));
        titleUml = titleUml.remove("|").remove(".");
        beginTitlePos = endTitlePos;
    }

    //umlaut가 있는 타이틀을 뽑아낸다.
    QStringList uml;
    uml<<"Ä"<<"Ö"<<"Ü"<<"ä"<<"ö"<<"ü"<<"ß";
    QStringListIterator itor(uml);

    //umlaut를 가지고 있는지 검사
    while(itor.hasNext()){
        if(titleUml.contains(itor.next().toUtf8().constData())){
            mltmapTitles.insert(titleUml, counterWord);
            break;
        }
    }//while(itor.hasNext())

    //전체 body문을 출력한다.
    //strlstDefinitions->append(pStr.trimmed());
    mapDefinitions.insert(counterWord, pStr.trimmed());

// 태그를 무시하고 내용만 읽어내는 코드
//    ui->plainTextEditDefinition->appendPlainText(reader.readElementText(QXmlStreamReader::IncludeChildElements));
//    if (reader.isEndElement()){
//        reader.readNext();
//    }
}

//6-6: 알려지지 않은 태그는 건너뛴다.
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

void MainWindow::on_listViewWord_clicked(const QModelIndex &index)
{
    QString word = index.data(Qt::DisplayRole).toString();
    ui->lineEditWord->clear();
    ui->lineEditWord->insert(word);
    QList<int> words = mltmapTitles.values(word);
    QListIterator<int> id(words);
    while(id.hasNext()){
        ui->plainTextEditDefinition->clear();
        ui->plainTextEditDefinition->appendHtml(mapDefinitions.value(id.next()));
    }
}
