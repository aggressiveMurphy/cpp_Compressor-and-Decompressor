#ifndef JIEYAWIDGET_H
#define JIEYAWIDGET_H

#include <QWidget>

namespace Ui {
class JieyaWidget;
}

class JieyaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit JieyaWidget(QWidget *parent = nullptr);
    ~JieyaWidget();

private slots:
    void on_pushButton_clicked();//打开压缩文件按钮槽函数

    void on_pushButton_2_clicked();//解压按钮槽函数

    void on_pushButton_3_clicked();//退出按钮槽函数

    void on_pushButton_4_clicked();//保存按钮槽函数

private:
    Ui::JieyaWidget *ui;
};

#endif // JIEYAWIDGET_H
