#include "ccwindowwidget.h"
#include <QDebug>


#define LANGCHERWIDGET_WIDTH 436
#define LANGCHERWIDGET_HEIGHT 859
CCWindowWidget::CCWindowWidget(QWidget *parent) : QWidget(parent)
{
    setWindowFlag(Qt::FramelessWindowHint);
    setFixedSize(LANGCHERWIDGET_WIDTH,LANGCHERWIDGET_HEIGHT);

    m_stackwidget=new CCStackWidget(this);
//m_stackwidget->CreateFrameWidgetPage(3);
    qDebug()<<"vccccccccccccccc "<<this->pos();

}

CCWindowWidget::~CCWindowWidget()
{

}
void CCWindowWidget::mousePressEvent(QMouseEvent *event)
{
     m_startPos=event->globalPos();

     m_pos=pos();

    return QWidget::mousePressEvent(event);
}
void CCWindowWidget::mouseMoveEvent(QMouseEvent *event)
{

//    QPoint pLast=event->pos();

    QPoint curPos=+event->globalPos()-m_startPos+m_pos;
    move(curPos);
    return QWidget::mouseMoveEvent(event);

}
void CCWindowWidget::mouseReleaseEvent(QMouseEvent *event)
{
    return QWidget::mouseReleaseEvent(event);
}
