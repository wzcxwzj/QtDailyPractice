#include "ccwindowwidget.h"



#define LANCHERWIDGET_WIDTH  436
#define LANCHERWIDGET_HEIGHT 775

#define TOOLBUTTON_WIDTH    75
#define TOOLBUTTON_HEIGHT   75

CCWindowWidget::CCWindowWidget(QWidget *parent) : QWidget(parent)
{
     m_StrechRuler=new CCStrechRuler(this);
     m_Dial=new CCDialTest(this);

     setFixedSize(LANCHERWIDGET_WIDTH,LANCHERWIDGET_HEIGHT);


     connect(m_StrechRuler,SIGNAL(rulerStrechSignalTest(float)),m_Dial,SLOT(onDialClockRulerStrechSignalTest(float)));

}
CCWindowWidget::~CCWindowWidget()
{
    if(m_StrechRuler != NULL)
    {
        delete m_StrechRuler;
        m_StrechRuler=NULL;
    }
}
