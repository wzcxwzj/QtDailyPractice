#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->widgetHeader->installEventFilter(this);
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->widgetHeader)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *event=(QMouseEvent *)event;
            if(event->buttons() & Qt::LeftButton)
            {
                m_lastPoint=event->globalPos();
            }
            //return true;
        }
        if(event->type() == QEvent::MouseMove)
        {
             QMouseEvent *event=(QMouseEvent *)event;
            if(event->buttons() & Qt::LeftButton)
            {
                 point=event->globalPos()-m_lastPoint;
                 ui->widgetHeader->move(ui->widgetHeader->pos().x(),point.y());
                // repaint();
            }
             // return true;
        }
//        if(event->type() == QEvent::Paint)
//        {
//            paintWidgetHerder(ui->widget_2);
//          return true;
//        }

    }
      return QWidget::eventFilter(watched,event);
}

//void Widget::mousePressEvent(QMouseEvent *event)
//{

//}

//void Widget::mouseMoveEvent(QMouseEvent *event)
//{

//}

//void Widget::mouseReleaseEvent(QMouseEvent *event)
//{

//}

void Widget::paintWidgetHerder(QWidget *wid)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);//设置反锯齿
    painter.setRenderHint(QPainter::SmoothPixmapTransform,true);//设置曲线平滑

    painter.save();
    painter.drawRect(10,5,ui->widgetHeader->width(),point.y());

    painter.restore();

}

