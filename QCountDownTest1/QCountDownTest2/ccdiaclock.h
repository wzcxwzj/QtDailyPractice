#ifndef CCDIACLOCK_H
#define CCDIACLOCK_H

#include <QWidget>
#include <QPainter>

class CCDiaClock : public QWidget
{
    Q_OBJECT
public:
    explicit CCDiaClock(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);
signals:

public slots:
    void oNsendRateRulerErDiaClock(float);
private:
    float m_roate;
};

#endif // CCDIACLOCK_H
