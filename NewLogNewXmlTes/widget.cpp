#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    qDebug()<<"9999999999999999999999999";
    qDebug()<<"888888888888888888888888";

    qDebug()<<"7777777777777777777777777";

    qDebug()<<"666666666666666666666666666";

    qDebug()<<"55555555555555555555555555";

}

Widget::~Widget()
{
    delete ui;
}

