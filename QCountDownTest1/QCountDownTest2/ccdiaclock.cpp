#include "ccdiaclock.h"

CCDiaClock::CCDiaClock(QWidget *parent) : QWidget(parent)
{
    setGeometry(75,188,285,285);
    m_roate=0.0;

}

void CCDiaClock::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform,true);

    painter.drawPixmap(rect(),QPixmap(":/images/ImageResources/dial.png"));

    painter.save();

    painter.translate(QPoint(width()/2,height()/2));

    painter.rotate(m_roate);
    painter.drawPixmap(QRect(-13,-78,26,90),QPixmap(":/images/ImageResources/needle.png"));
    painter.restore();

}
void CCDiaClock::oNsendRateRulerErDiaClock(float value)
{
    m_roate=360*value;
}
