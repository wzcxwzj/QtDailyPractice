#include "ccrulerheader.h"




#define RULERHEADER_WIDTH  82
#define RULERHEADER_HEIGHT 91

CCRulerHeader::CCRulerHeader(QWidget *parent) : QWidget(parent)
{
    setGeometry(0,0,RULERHEADER_WIDTH,RULERHEADER_HEIGHT);
}
void CCRulerHeader::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.save();
    painter.drawPixmap(rect(),QPixmap(":/ImageResources/ruler_head.png"));
    painter.restore();
}
void CCRulerHeader::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_lastPoint=event->globalPos();
    }
}
void CCRulerHeader::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        QPoint point=event->globalPos()-m_lastPoint;
        emit rulerMoveDistance(point);
    }
}
void CCRulerHeader::mouseReleaseEvent(QMouseEvent *event)
{
    emit rulerMoveDistanceDone();
}
