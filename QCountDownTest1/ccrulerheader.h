#ifndef CCRULERHEADER_H
#define CCRULERHEADER_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>

class CCRulerHeader : public QWidget
{
    Q_OBJECT
public:
    explicit CCRulerHeader(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void rulerMoveDistance(QPoint);
    void rulerMoveDistanceDone();

public slots:
private:
    QPoint m_lastPoint;

};

#endif // CCRULERHEADER_H
