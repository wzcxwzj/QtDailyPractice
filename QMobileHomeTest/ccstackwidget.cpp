#include "ccstackwidget.h"

#include "ccframewdiegt.h"

#include <QDebug>

#define STACKWIDGET_WIDTH 375
#define STACKWIDGET_HEIGHT 750
CCStackWidget::CCStackWidget(QWidget *parent) : QWidget(parent)
{
    setGeometry(30,55,STACKWIDGET_WIDTH,STACKWIDGET_HEIGHT);

     m_curIndex=0;
     m_nextIndex=1;

     m_maxDistance=0;
     CreateFrameWidgetPage(3);

}

void CCStackWidget::CreateFrameWidgetPage(int count)
{

    for (int var = 0; var < count; var++) {
        CCFrameWdiegt *frameWdidget=new CCFrameWdiegt(this);
        frameWdidget->setGeometry(0,0,STACKWIDGET_WIDTH,STACKWIDGET_HEIGHT);

        connect(frameWdidget,SIGNAL(mouseMoveDistance(QPoint)),this,SLOT(onMouseMoveDistance(QPoint)));
        connect(frameWdidget,SIGNAL(mouseMoveDistanceDone()),this,SLOT(onMouseMoveDistanceDone()));

#if 1
        frameWdidget->setAutoFillBackground(true);
        QPalette palette(frameWdidget->palette());
        if(var ==0) palette.setColor(QPalette::Background,Qt::blue);
        if(var ==1) palette.setColor(QPalette::Background,Qt::green);
        if(var ==2) palette.setColor(QPalette::Background,Qt::red);
        frameWdidget->setPalette(palette);
#endif

        if(var == 0)
        {
            frameWdidget->setHidden(false);
        }else {
            frameWdidget->setHidden(true);
        }
        m_frameWidgetList.append(frameWdidget);
    }
}


int CCStackWidget::getCount()
{
    return m_frameWidgetList.count();
}
int CCStackWidget::getCurIndex()
{
    return m_curIndex;
}
CCFrameWdiegt *CCStackWidget::getCurFrameWidget()
{

     CCFrameWdiegt *frameWdidget=m_frameWidgetList.at(m_curIndex);
     return frameWdidget;
}

CCFrameWdiegt *CCStackWidget::getIndexFrameWidget(int Index)
{
//    for (int var = 0; var < m_frameWidgetList.size(); var++)
//    {
        CCFrameWdiegt *frameWdidget=(CCFrameWdiegt*)m_frameWidgetList.at(Index);
        return frameWdidget;
//    }
}

void CCStackWidget::setCurIndex(int Index)
{
    for (int var = 0; var < m_frameWidgetList.size(); var++) {
        CCFrameWdiegt *frameWdidget= m_frameWidgetList.at(var);
        frameWdidget->setGeometry(0,0,STACKWIDGET_WIDTH,STACKWIDGET_HEIGHT);
        if(var == Index)
        {
            frameWdidget->setHidden(false);
        }else {
            frameWdidget->setHidden(true);
        }
    }
   m_curIndex=Index;
}

void CCStackWidget::onMouseMoveDistance(QPoint pos)
{
    float distanceVale= pos.x();
    int pageTotal=getCount();
    int curIndex=getCurIndex();

    if(distanceVale <-60)
    {
        ++curIndex;
    }
    else if (distanceVale >= 60) {
        --curIndex;
    }

    if(curIndex >= pageTotal)
    {
        curIndex=pageTotal-1;

    }else if(curIndex <= 0) {
        curIndex=0;
    }

    m_nextIndex=curIndex;
    updateMoveDistance(distanceVale);
}
void CCStackWidget::updateMoveDistance(float distance)
{
    if(m_maxDistance == 0)
    {
        m_maxDistance= distance;
    }
    CCFrameWdiegt *currentFrameWidget=getCurFrameWidget();
    CCFrameWdiegt *nextFrameWidget=getIndexFrameWidget(m_nextIndex);
    nextFrameWidget->setHidden(false);

    if(distance < 0)
    {

        if(distance< m_maxDistance)
        {   //当前的页面  framewidget->move(distance,0);
            currentFrameWidget->move(distance,0);
            if(m_curIndex != m_nextIndex)
            {
                //下一个页面 nextframewidget->move(STACKWIDGET_WIDTH-qabs(distance),0);
                nextFrameWidget->move(STACKWIDGET_WIDTH-qAbs(distance),0);
            }
            m_maxDistance=distance;
        }
    }
    else if(distance > 0)
    {

        if(distance > m_maxDistance)
        {
            //当前的页面  framewidget->move(distance,0);
            currentFrameWidget->move(distance,0);
            if(m_curIndex != m_nextIndex)
            {
                //下一个页面 nextframewidget->move(qabs(distance)-STACKWIDGET_WIDTH,0);
                nextFrameWidget->move(qAbs(distance)-STACKWIDGET_WIDTH,0);
            }
            m_maxDistance=distance;
        }
    }


}
void CCStackWidget::FinishMoveDone()
{
    setCurIndex(m_nextIndex);
    m_maxDistance =0;

}
void CCStackWidget::onMouseMoveDistanceDone()
{
    FinishMoveDone();
}




















