#ifndef CCSTACKWIDGET_H
#define CCSTACKWIDGET_H

#include <QWidget>


class CCFrameWdiegt;
class CCStackWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CCStackWidget(QWidget *parent = nullptr);

    void CreateFrameWidgetPage(int count);
    int getCount();
    int getCurIndex();
    CCFrameWdiegt *getCurFrameWidget();
    CCFrameWdiegt *getIndexFrameWidget(int Index);
    void setCurIndex(int Index);


    void updateMoveDistance(float distance);
    void FinishMoveDone();
signals:

public slots:

private slots:
    void onMouseMoveDistance(QPoint);
    void onMouseMoveDistanceDone();
private:
    QList<CCFrameWdiegt *>m_frameWidgetList;

    int m_curIndex;
    int m_nextIndex;

    int m_maxDistance;
};

#endif // CCSTACKWIDGET_H
