#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_pushButton_clicked();//压缩源程序按钮槽函数

    void on_pushButton_2_clicked();//解压文件按钮槽函数
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
