#ifndef CCSTRECHRULER_H
#define CCSTRECHRULER_H

#include <QWidget>
#include <QPainter>
#include "ccrulerheader.h"

class CCStrechRuler : public QWidget
{
    Q_OBJECT
public:
    explicit CCStrechRuler(QWidget *parent = nullptr);
    virtual ~CCStrechRuler();
    void paintEvent(QPaintEvent *event);
signals:
    void rulerStrechSignalTest(float);
public slots:
    void onRulerMoveDistance(QPoint);
    void onRulerMoveDistanceDone();
private:
    CCRulerHeader *m_rulerHeader;
    QPoint          m_originalPosition;
    int             m_currentPosY;

};

#endif // CCSTRECHRULER_H
