#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_cdiaclock=new CCDiaClock(this);

    connect(this,SIGNAL(sendRateRulerErDiaClock(float)),m_cdiaclock,SLOT(oNsendRateRulerErDiaClock(float)));
}

Widget::~Widget()
{
    if(m_cdiaclock != NULL)
    {
        delete m_cdiaclock;
        m_cdiaclock=NULL;
    }

    delete ui;
}


void Widget::mousePressEvent(QMouseEvent *event)
{

    if(event->buttons() & Qt::LeftButton)
    {
        m_lastPoint=event->globalPos();
    }

}

void Widget::mouseMoveEvent(QMouseEvent *event)
{

    if(event->buttons() & Qt::LeftButton)
    {
        point=event->globalPos()-m_lastPoint;
        ui->widgetHeader->move(ui->widgetHeader->pos().x(),point.y());

    }

    float  rate=1.0*point.y()/ui->widget_2->height();

//    qDebug()<<"ssssss"<<rate;

    emit sendRateRulerErDiaClock(rate);
    repaint();
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{

}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);//设置反锯齿
    painter.setRenderHint(QPainter::SmoothPixmapTransform,true);//设置曲线平滑

    painter.save();
    painter.drawRect(ui->widget_2->x()+20,5,ui->widgetHeader->width(),point.y());

    painter.restore();

    int step=5;
    int bigStep=25;

    int headerTotal=point.y();

    int stepTotal=headerTotal/step;
    int bigStepTotal=headerTotal/bigStep;


    int recWidth=ui->widget_2->x()+20 + ui->widgetHeader->width();

    for (int var = 0; var < stepTotal; ++var) {

            if(var%5 != 0)
            {
                int xNum1=(recWidth-5);

                int yNum1=var*5;

                int xNum2=recWidth;

                int yNum2=yNum1;
                painter.drawLine(xNum1,yNum1,xNum2,yNum2);

            }

    }
    for (int var = 0; var < bigStepTotal; ++var) {

        int xNum1=(recWidth-10);

        int yNum1=var*25;

        int xNum2=recWidth;

        int yNum2=yNum1;
        painter.drawLine(xNum1,yNum1,xNum2,yNum2);

    }

}


