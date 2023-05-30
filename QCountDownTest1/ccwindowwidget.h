#ifndef CCWINDOWWIDGET_H
#define CCWINDOWWIDGET_H

#include <QWidget>
#include "ccstrechruler.h"
#include "ccdialtest.h"

class CCWindowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CCWindowWidget(QWidget *parent = nullptr);

    virtual ~CCWindowWidget();
signals:

public slots:
private:
    CCStrechRuler *m_StrechRuler;
    CCDialTest *m_Dial;
};

#endif // CCWINDOWWIDGET_H
