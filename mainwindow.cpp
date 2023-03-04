#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "subwidget.h"//压缩子窗口头文件
#include "jieyawidget.h"//解压子窗口头文件

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{//压缩源程序按钮槽函数
    this->close();
    SubWidget *s=new SubWidget();
    s->show();
}

void MainWindow::on_pushButton_2_clicked()
{//解压文件按钮槽函数
    this->close();
    JieyaWidget *j=new JieyaWidget();
    j->show();
}
