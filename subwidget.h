#ifndef SUBWIDGET_H
#define SUBWIDGET_H

#include <QWidget>

namespace Ui {
class SubWidget;
}

class SubWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SubWidget(QWidget *parent = nullptr);
    ~SubWidget();

private slots:
    void on_pushButton_clicked();//打开源文件按钮槽函数

    void on_pushButton_2_clicked();//压缩按钮槽函数

    void on_pushButton_3_clicked();//保存按钮槽函数

    void on_pushButton_4_clicked();//退出按钮槽函数

private:
    Ui::SubWidget *ui;
};

#endif // SUBWIDGET_H
