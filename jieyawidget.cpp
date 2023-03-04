#include "jieyawidget.h"
#include "ui_jieyawidget.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "mainwindow.h"

JieyaWidget::JieyaWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JieyaWidget)
{
    ui->setupUi(this);
}

JieyaWidget::~JieyaWidget()
{
    delete ui;
}
QMap<QString,int> word_to_id1;
QMap<int,QString> id_to_word1;
QString Line1[100000];
int g1;
QString decompression_char[100000];
int o=-1;
void build_sheet1()
{//建立字符编号对照表
    QString file_path="../gccWords.txt";//读取已经建立好的压缩表
    QFile file(file_path);
    if(!file.open(QFile::ReadOnly | QFile::Text)) return;
    QTextStream in(&file);
    while(!in.atEnd()){
        QString line=in.readLine();
        int len=line.length();
        int l=0;
        QString number="";
        QString char_S="";
        QByteArray byte=line.toUtf8();
        while(l<len&&byte.at(l)!=' '){
            if ('0'<=byte.at(l)&&byte.at(l)<='9') number+=byte.at(l);
            l++;
        }
        l++;
        while(l<len){
            char_S+=byte.at(l);
            l++;
        }
        QString str(number);
        int number_int=str.toInt();
        word_to_id1[char_S]=number_int;
        id_to_word1[number_int]=char_S;
        qDebug() << number_int << ' ' << char_S;
    }
}

void JieyaWidget::on_pushButton_clicked()
{//打开文件
    QString file_path = QFileDialog::getOpenFileName(this,"选择一个代码文件", ".", "Code(*.txt *.cpp )");
    ui->lineEdit->setText(file_path);
    ui->textEdit->clear();
    QFile file(file_path);
    if(!file.open(QFile::ReadOnly | QFile::Text)) return;
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine();
        Line1[g1]=line;
        g1++;
        ui->textEdit->append(line);
    }
    g1--;
    file.close();
    build_sheet1();
}

void JieyaWidget::on_pushButton_2_clicked()
{//解压
    ui->textEdit_2->clear();
    for (int i=0;i<=g1;i++){
        QByteArray byte=Line1[i].toUtf8();
        int L=Line1[i].size();
        int l=0;
        QString decompression_S="";
        while(l<L){
            if (byte.at(l)!=' '){
                QString s="";
                while((l<L)&&('0'<=byte.at(l))&&(byte.at(l)<='9')){
                    s+=byte.at(l);
                    l++;
                }
                QString str(s);
                int index=str.toInt();
                decompression_S+=id_to_word1[index];
            }
            else l++;
        }
        qDebug() << decompression_S << endl;
        if (decompression_S!=""){
            o++;
            ui->textEdit_2->append(decompression_S);
            decompression_char[o]=decompression_S;
        }
    }
}
void JieyaWidget::on_pushButton_3_clicked()
{//退出，返回主窗口
    this->close();
    MainWindow *w=new MainWindow();
    w->show();
}
void JieyaWidget::on_pushButton_4_clicked()
{//保存，将解压后的内容写入解压文件
    QString file_path = QFileDialog::getSaveFileName(this,"保存文件","","Text(*.txt);;all(*.*)");//解压后写入解压文件
    QFile f1(file_path);
    if(!f1.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)){//追加写入 添加结束符\r\n
        QMessageBox::warning(this,tr("错误"),tr("打开文件失败,数据保存失败"));
        return ;
    }
    else{
        QTextStream stream1(&f1);
        for (int j=0;j<=o;j++) stream1 << decompression_char[j] << endl;
    }
    f1.close();
}
