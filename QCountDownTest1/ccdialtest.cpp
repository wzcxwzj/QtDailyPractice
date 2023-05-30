#include "ccdialtest.h"


#define DIALCLOCK_STARTX 75
#define DIALCLOCK_STARTY 188
#define DIALCLOCK_WIDTH  285
#define DIALCLOCK_HEIGHT 285

#define NEEDLE_WIDTH  26
#define NEEDLE_HEIGHT 90


CCDialTest::CCDialTest(QWidget *parent) : QWidget(parent)
{
    setGeometry(DIALCLOCK_STARTX,DIALCLOCK_STARTY,DIALCLOCK_WIDTH,DIALCLOCK_HEIGHT);
    m_roate = 0.0;

}

void CCDialTest::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform,true);
    painter.drawPixmap(rect(),QPixmap(":/ImageResources/dial.png"));

    painter.save();

    painter.translate(width()/2,height()/2);
    painter.rotate(m_roate);
    painter.drawPixmap(QRect(-13,-78,NEEDLE_WIDTH,NEEDLE_HEIGHT),QPixmap(":/ImageResources/needle.png"));
    painter.restore();
}
void CCDialTest::onDialClockRulerStrechSignalTest(float xvalue)
{
    m_roate=360*xvalue;

     repaint();
}
