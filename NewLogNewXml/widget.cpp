#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    qDebug()<<"99999999999999";
    qDebug()<<"88888888888888";

    qDebug()<<"77777777777777";

    qDebug()<<"66666666666666";

    qDebug()<<"55555555555555";

    qDebug()<<"44444444444444";

    qDebug()<<"33333333333333";

}

Widget::~Widget()
{ qDebug()<<"99999999999999";
    delete ui;
}

