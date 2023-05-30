#ifndef CCWINDOWWIDGET_H
#define CCWINDOWWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include "ccstackwidget.h"


class CCWindowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CCWindowWidget(QWidget *parent = nullptr);
    virtual ~CCWindowWidget();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:

public slots:

private:
    CCStackWidget *m_stackwidget;

    QPoint m_startPos;
    QPoint m_pos;

};

#endif // CCWINDOWWIDGET_H
