#ifndef CCFRAMEWDIEGT_H
#define CCFRAMEWDIEGT_H

#include <QWidget>
#include <QMouseEvent>

class CCFrameWdiegt : public QWidget
{
    Q_OBJECT
public:
    explicit CCFrameWdiegt(QWidget *parent = nullptr);
    virtual ~CCFrameWdiegt();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:

signals:
    void mouseMoveDistance(QPoint);
    void mouseMoveDistanceDone();


public slots:

private:
    QPoint m_staPos; //黄山 西宁 怀化
};

#endif // CCFRAMEWDIEGT_H
