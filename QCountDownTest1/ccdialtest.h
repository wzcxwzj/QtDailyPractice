#ifndef CCDIALTEST_H
#define CCDIALTEST_H

#include <QWidget>
#include <QPainter>

class CCDialTest : public QWidget
{
    Q_OBJECT
public:
    explicit CCDialTest(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event);
signals:

public slots:

    void onDialClockRulerStrechSignalTest(float);
private:

    float m_roate;
};

#endif // CCDIALTEST_H
