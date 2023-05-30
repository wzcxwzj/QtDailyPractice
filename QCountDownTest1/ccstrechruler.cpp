#include "ccstrechruler.h"
#include <QDebug>


#define RULERWIDGET_STARTX 351
#define RULERWIDGET_STARTY 100

#define RULERWIDGET_WIDTH  82
#define RULERWIDGET_HEIGHT 574

#define RULER_MAX_HEIGHT 486

CCStrechRuler::CCStrechRuler(QWidget *parent) : QWidget(parent)
{
    setGeometry(RULERWIDGET_STARTX,RULERWIDGET_STARTY,RULERWIDGET_WIDTH,RULERWIDGET_HEIGHT);

    m_rulerHeader=new CCRulerHeader(this);

    connect(m_rulerHeader,SIGNAL(rulerMoveDistance(QPoint)),this,SLOT(onRulerMoveDistance(QPoint)));
    connect(m_rulerHeader,SIGNAL(rulerMoveDistanceDone()),this,SLOT(onRulerMoveDistanceDone()));
    m_originalPosition=m_rulerHeader->pos();
    m_currentPosY = m_originalPosition.ry();

}
CCStrechRuler::~CCStrechRuler()
{
    if(m_rulerHeader!= NULL)
    {
        delete m_rulerHeader;
        m_rulerHeader=NULL;
    }

}
void CCStrechRuler::paintEvent(QPaintEvent *event)
{
      QPainter painter(this);
      //painter.setRenderHint();//设置反锯齿
                                //设置平滑曲线

      painter.save();
      painter.setPen(QPen(QColor(127, 159, 124), 1));
      painter.setBrush(QColor(127, 159, 124));
      painter.drawRect(15, 3, 52, m_rulerHeader->pos().y());
      painter.restore();


       painter.save();
       int step=5;
       int bigStep=25;



       int moveDistance=m_rulerHeader->pos().y();

       int stepShort    = moveDistance/step    +1;
       int bigStepTotal = moveDistance/bigStep +1;

       painter.setPen(QPen(QColor(255, 255, 255),1));

       for (int var = 0; var < stepShort; var++) {

           if(var%5 != 0)
            {
               int xnum1=(67-5);
               int ynum1=5*var;
               int xnum2=67;
               int ynum2=ynum1;
               painter.drawLine(xnum1,ynum1,xnum2,ynum2);
           }
       }
       for (int var = 0; var < bigStepTotal; var++) {
           int xnum1=(67-10);
           int ynum1=25*var;
           int xnum2=67;
           int ynum2=ynum1;
           painter.drawLine(xnum1,ynum1,xnum2,ynum2);
       }


     //  painter.drawLine(xnum1,ynum1,xnum2,ynum2);

}
void CCStrechRuler::onRulerMoveDistance(QPoint point)
{

    if(m_rulerHeader == NULL){
        return;
    }
    int aValue=point.y();
    int realPosY= m_currentPosY + aValue;

    if((realPosY <= RULER_MAX_HEIGHT) && (realPosY >= 0))
    {

         m_rulerHeader->move(m_originalPosition.rx(),realPosY);

    }

    float dialRatio = m_rulerHeader->pos().y() / 486.0;

    emit rulerStrechSignalTest(dialRatio);
    repaint();

   // m_rulerHeader->move(point.x(),point.y());
    qDebug()<<"--------- "<<point;
}
void CCStrechRuler::onRulerMoveDistanceDone()
{
qDebug()<<"==========";
}
