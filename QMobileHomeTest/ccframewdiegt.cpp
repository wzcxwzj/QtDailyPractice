#include "ccframewdiegt.h"



CCFrameWdiegt::CCFrameWdiegt(QWidget *parent) : QWidget(parent)
{

}
CCFrameWdiegt::~CCFrameWdiegt()
{

}

void CCFrameWdiegt::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_staPos=event->pos();
    }
}

void CCFrameWdiegt::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        QPoint point = event->pos()-m_staPos;
        emit mouseMoveDistance(point);
    }
}

void CCFrameWdiegt::mouseReleaseEvent(QMouseEvent *event)
{

    emit mouseMoveDistanceDone();

}
