#include "subwidget.h"
#include "ui_subwidget.h"
#include <QMap>
#include <QString>
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <mainwindow.h>

QMap<QString,int> word_to_id;//压缩表，将关键字和运算符等编号,通过字符查找编号
QMap<int,QString> id_to_word;//解压表，通过编号查找字符
int index=0;//索引
SubWidget::SubWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SubWidget)
{
    ui->setupUi(this);
}

SubWidget::~SubWidget()
{
    delete ui;
}

QString Line[100000];
QString All_S[10000];
int hang=0;
int g=0;
void build_sheet()
{//建立压缩表
    QString file_path="./dict_file.txt";
    QFile file(file_path);
    if(!file.open(QFile::ReadOnly | QFile::Text)) return;
    QTextStream in(&file);//从文件中读入数据
    int exist=0;
    while(!in.atEnd()){
        QString line=in.readLine();
        int len=line.length();
        int l=0;
        QString number="";//存放数字的字符串
        QString char_S="";//存放字符的字符串
        QByteArray byte=line.toUtf8();
        while(l<len&&byte.at(l)!=' '){
            //读取数据，如果是数字0-9则将该数字放进number字符串
            if ('0'<=byte.at(l)&&byte.at(l)<='9') number+=byte.at(l);
            l++;
        }
        l++;
        while(l<len){
            //将字符放进char_S字符串
            char_S+=byte.at(l);
            l++;
        }
        QString str(number);
        int number_int=str.toInt();
        exist=1;
        word_to_id[char_S]=number_int;//将number存放的数字转换成整型数
        id_to_word[number_int]=char_S;
        if (number_int>=index) index=number_int;//给字符编号
        qDebug() << number_int << ' ' << char_S;
    }
    if (exist==1) index++;
}
void SubWidget::on_pushButton_clicked()
{//打开源文件并展示文件内容
    QString file_path = QFileDialog::getOpenFileName(this,"选择一个代码文件", ".", "Code(*.txt *.cpp )");
    ui->lineEdit->setText(file_path);//展示源程序路径

    ui->textEdit->clear();//清除文本框内上一次留下的文本内容
    QFile file(file_path);
    g=0;
    if(!file.open(QFile::ReadOnly | QFile::Text)) return;
    QTextStream in(&file);

    while(!in.atEnd())
    {//读取文件数据
        QString line = in.readLine();
        Line[g]=line;
        g++;
        ui->textEdit->append(line);//展示文件内容
    }
    g--;
    file.close();
    build_sheet();//建立字符对应编号表
}

void SubWidget::on_pushButton_2_clicked()
{//压缩：查找字符在对应的压缩表中的编号并展示在文本框中
    QString S="";
    QString intS="";
    QString str="";
    int nothing=0;//判断是否为块注释的标志
    int addspace=0;
    hang=0;
    ui->textEdit_2->clear();
    for(int i=0;i<=g;i++){
        QByteArray byte=Line[i].toUtf8();
        intS="";
        int L=Line[i].size();
        int l=0;
        if (Line[i]=="") continue;
        int space=1;//判断是否为空格的标志
        while(l<L){
            while(nothing==1){
                // 寻找*/
                //qDebug()<<nothing<<L<<l<<' '<<byte.at(l)<<"-------"<<byte.at(l+1);
                if (l+1>=L) break;
                if (l+1<L&&byte.at(l)=='*'&&byte.at(l+1)=='/'){
                    nothing=0;
                    l++;
                    QString s="";//属于行注释部分，直接忽略
                }
                l++;
            }
            if (nothing==1&&l+1>=L) {
                //属于块注释，直接忽略该行
                QString s="";
                break;
            }
            if (l>=L) continue;
            QString s="";//存放字符的字符串，后续要查找是否为关键字或者标识符
            if (space==1){
                //有空格，跳过处理下一个字符,即将空格当成分隔符
                while(byte.at(l)==' ') l++;
            }
            switch (byte.at(l)) {
            case '#':{
                //#后面跟的可能是include或者define或者其他
                space=0;
                s+=byte.at(l);l++;
                while((l<L)&&('a'<=byte.at(l)&&byte.at(l)<='z')){
                    s+=byte.at(l);
                    l++;
                }
                break;
            }
            case 'a'...'z':{
                //小写字母a到z后面跟的可能是下划线，大写或小写字母，或者数字0到9
                space=0;
                s+=byte.at(l);l++;
                while((l<L)&&((byte.at(l)=='_')||('a'<=byte.at(l)&&byte.at(l)<='z')||('A'<=byte.at(l)&&byte.at(l)<='Z')||('0'<=byte.at(l)&&byte.at(l)<='9'))){
                        s+=byte.at(l);
                        l++;
                }
                break;
            }
            case 'A'...'Z':{
                //大写字母A到Z后面跟的可能是下划线，大写或小写字母，或者数字0到9
                space=0;
                s+=byte.at(l);l++;
                while((l<L)&&((byte.at(l)=='_')||('a'<=byte.at(l)&&byte.at(l)<='z')||('A'<=byte.at(l)&&byte.at(l)<='Z')||('0'<=byte.at(l)&&byte.at(l)<='9'))){
                    s+=byte.at(l);
                    l++;
                }
                break;
            }
            case '0'...'9':{
                //数字0-9后面跟的可能是数字0-9
                //数字0-9可能是十进制数可能是十六进制数
                space=0;
                s+=byte.at(l);l++;
                if ((l<L)&&(byte.at(l)=='x'||byte.at(l)=='X')) {
                    //0x或者0X表示十六进制数
                    s+=byte.at(l);
                    l++;
                }
                while((l<L)&&('0'<=byte.at(l))&&(byte.at(l)<='9')){
                    s+=byte.at(l);
                    l++;
                }
                break;
            }
            case '<':{
                space=0;
                s+=byte.at(l);l++;
                //<<,<=,<<=
                while((l<L)&&((byte.at(l)=='<')||(byte.at(l)=='=')))
                {
                    s+=byte.at(l);
                    l++;
                }
                break;
            }
            case '>':{
                space=0;
                s+=byte.at(l);l++;
                //>>,>=,>>=
                while((l<L)&&((byte.at(l)=='>')||(byte.at(l)=='=')))
                {
                    s+=byte.at(l);
                    l++;
                }
                break;
            }
            case '.':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            case '{':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            case '}':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            case '(':{
                space=0;
                s+=byte.at(l);l++;
                if (l>=L) break;
                switch (byte.at(l)){
                //()
                case ')':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                }
                break;
            }
            case ')':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            case '[':{
                space=0;
                s+=byte.at(l);l++;
                if (l>=L) break;
                switch (byte.at(l)){
                //[]
                case ']':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                }
                break;
            }
            case ']':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            case '+':{
                space=0;
                s+=byte.at(l);l++;
                if (l>=L) break;
                switch (byte.at(l)){
                //+=
                case '=':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                //++
                case '+':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                }
                break;
            }
            case '-':{
                space=0;
                s+=byte.at(l);l++;
                if (l>=L) break;
                switch (byte.at(l)) {
                //-=
                case '=':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                //--
                case '-':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                //->
                case '>':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                }
                break;
            }
            case '*':{
                space=0;
                s+=byte.at(l);l++;
                if (l>=L) break;
                switch (byte.at(l)) {
                //*=
                case '=':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                }
                break;
            }
            case '/':{
                space=0;
                s+=byte.at(l);l++;
                if (l>=L) break;
                switch (byte.at(l)) {
                case '/':{
                    // //行注释，该行可以忽略
                    l=L;
                    s.chop(1);
                    break;
                }
                case '=':{
                    // /=
                    s+=byte.at(l);
                    l++;
                    break;
                }
                case '*':{
                    // /*块注释
                    s.chop(1);
                    l++;
                    nothing=1;
                    break;
                }
                }
                break;
            }
            case '%':{
                space=0;
                s+=byte.at(l);l++;
                if (l>=L) break;
                switch (byte.at(l)) {
                //%=
                case '=':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                }
                break;
            }
            case '=':{
                space=0;
                s+=byte.at(l);l++;
                if (l>=L) break;
                switch (byte.at(l)) {
                //==
                case '=':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                }
                break;
            }
            case '&':{
                space=0;
                s+=byte.at(l);l++;
                if (l>=L) break;
                switch (byte.at(l)) {
                //&=
                case '=':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                //&&
                case '&':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                }
                break;
            }
            case '^':{
                space=0;
                s+=byte.at(l);l++;
                if (l>=L) break;
                switch (byte.at(l)) {
                //^=
                case '=':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                }
                break;
            }
            case '|':{
                space=0;
                s+=byte.at(l);l++;
                if (l>=L) break;
                switch (byte.at(l)) {
                //|=
                case '=':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                //||
                case '|':{
                    s+=byte.at(l);
                    l++;
                    break;
                }
                }
                break;
            }
            case '$':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            case '@':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            case '!':{
                space=0;
                s+=byte.at(l);l++;
                if (l>=L) break;
                if (byte.at(l)=='='){
                    // !=
                    s+=byte.at(l);
                    l++;
                }
                else{
                    // !后跟下划线，或者大小写字母或者数字0-9
                    if (('a'<=byte.at(l)&&byte.at(l)<='z')||('A'<=byte.at(l)&&byte.at(l)<='Z')){
                        while((l<L)&&((byte.at(l)=='_')||('a'<=byte.at(l)&&byte.at(l)<='z')||('A'<=byte.at(l)&&byte.at(l)<='Z')||('0'<=byte.at(l)&&byte.at(l)<='9'))){
                                s+=byte.at(l);
                                l++;
                        }
                    }
                    if ('0'<=byte.at(l)&&byte.at(l)<='9'){
                        while((l<L)&&('0'<=byte.at(l)&&byte.at(l)<='9')){
                                s+=byte.at(l);
                                l++;
                        }
                    }
                }
                break;
            }
            case ';':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            case '~':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            case ',':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            case ':':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            case '"':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            case ' ':{
                s+=byte.at(l);l++;
                break;
            }
            case '?':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            case '`':{
                space=0;
                s+=byte.at(l);l++;
                break;
            }
            default:{
                if (int(byte.at(l))==39){
                    s+=byte.at(l);l++;
                    break;
                }
                l++;
            }
            }
            if (s!=""){  // '
                 addspace=1;
                 if (!word_to_id.contains(s)){
                     //不是关键字
                     word_to_id[s]=index;
                     id_to_word[index]=s;
                     index++;
                 }
                 str=QString::number(word_to_id[s]);
                 S+=s;
                 intS=intS+str+' ';
            }
        }
        if (nothing==0&&addspace==1){
             S+=' ';
        }
        addspace=0;
        //去除末尾多余空格
        int len=S.size();
        QByteArray byte1=S.toUtf8();
        while(len-2>=0&&byte1.at(len-1)==' '&&byte1.at(len-2)==' '){
            S.chop(1);
            len--;
        }
        len=intS.size();
        byte1=intS.toUtf8();
        while(len-1>=0&&byte1.at(len-1)==' '){
            intS.chop(1);//移除最后一位
            len--;
        }
        if (intS!=""){
            All_S[hang]=intS;
            hang++;
            ui->textEdit_2->append(intS);//将压缩文件内容展示在文本框内
        }
    }
}

void SubWidget::on_pushButton_3_clicked()
{//保存，将压缩后的内容写入压缩文件
    QString file_path = QFileDialog::getSaveFileName(this,"保存文件","","Text(*.txt);;all(*.*)");
    QFile f1(file_path);
    if(!f1.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)){//追加写入 添加结束符\r\n
        QMessageBox::warning(this,tr("错误"),tr("打开文件失败,数据保存失败"));
        return ;
    }
    else{
        QTextStream stream(&f1);
        for (int i=0;i<=hang-1;i++){
            stream << All_S[i] << endl;
        }
    }
    f1.close();
    file_path = "../gccWords.txt";//同时保存压缩表
    QFile f2(file_path);
    if(!f2.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){//QIODevice::Append追加写入 添加结束符\r\n
        QMessageBox::warning(this,tr("错误"),tr("打开文件失败,数据保存失败"));
        return ;
    }
    else{
        QTextStream stream(&f2);
        QMapIterator<QString, int> iterator(word_to_id);
        while (iterator.hasNext()) {
            iterator.next();
            stream << iterator.value() << ' ' << iterator.key() << endl;
        }

    }
    f2.close();
}

void SubWidget::on_pushButton_4_clicked()
{//退出当前窗口，返回主窗口
    this->close();
    MainWindow *w=new MainWindow();
    w->show();
}

