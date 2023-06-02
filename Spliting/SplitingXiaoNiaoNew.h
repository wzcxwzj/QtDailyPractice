#ifndef SplitingXiaoNiaoNew_H
#define SplitingXiaoNiaoNew_H
#include "wm.h"
#include "CNetWorkForClients.h"
#include "CNetWorkForClientsUdp.h"
#include "CConfig.h"

#include <QQueue>
#include <QTimer>
#define MSG_SEND_SPAKING 30 //消息队列发送间隔 单位毫秒
#define SYNC_TIMERMSEC 10 //同步间隔 单位秒
#define MESSAGE_QUEUE_CAPACITY 100//消息队列默认数量


class SplitingXiaoNiaoNew:public WM ,public CNetWorkInterface
{
    Q_OBJECT
public:
    SplitingXiaoNiaoNew(QString strIp,int nPort);
public:
    virtual WM_PROPERTY getWindowByPos(int sid ,int x, int y); //查找某个坐标下的窗口
    virtual bool isTopWindow(WM_PROPERTY &h);                  //是否为最顶层窗口
    virtual void setSyncFrequency(int s);                      //设置数据同步时间
    virtual WM_LIST findWindowByChennel(int sid, int nid);     //查找某个通道在某组屏幕上的所有开窗
    virtual void reconnectionServer(const QString& serverIp ,const int &nport);//链接服务器,可多重连。
    virtual void dataSync();//手动同步数据
    virtual void setWindowPos(const WM_PROPERTY &h,const WM_RECT &rect);//移动窗口(h.sid,h.nid,h.wid必须有效)
    virtual void setWindowOnTop(const WM_PROPERTY &h);//将窗口置顶(h.sid,h.wid必须有效)
    virtual void closeWindow(const WM_PROPERTY &h);//删除窗口(h.sid,h.wid必须有效)
    virtual void callScene(int sid, int n);//调取某组屏的第n号场景
    virtual void openWindow(int sid,int nid,const WM_RECT &rect);//将某一信号通道开窗到某组屏的具体某个矩形区域
    virtual void cleanAllWindow();//删除所有屏窗口
    virtual void cleanGropWindow(int sid);//删除某一组屏的所有窗口
    void setMessageQueueCapacity(int ncapacity);//设置消息队列容量
    void openWindow1(int sid, int nid, QList<WM_PROPERTY> listRect);//将某一信号通道开窗到某组屏的具体某个矩形区域
    void callSceneList(int sid, int n);//调取某组屏的第n号场景

    void readXML();
    void readXMLScreenId();
    void delay_msec(unsigned int msec);
    void openWindowTest(int sid, int iwid, QString nid, int xr, int yr, int iWidth, int height);
signals:
    virtual void dataReady(const QByteArray& bys);//同步完成
    virtual void deviceError();//拼控连接错误
            void sigStartTimer();//跨线程通过信号启动定时器过一定时间后发送同步数据

            void testSingleSourceOpenChange(WM_LIST);
            void testSingleSourceMoveChange(int,int,WM_LIST);
private:
    virtual void dataArrived(QByteArray arrMessage,QString strIp,int nport);
    virtual void connectStateChanged(bool bstate,QString strIp,int nport);
private:
    void InitSyncInfo();
    void pushMesageQueue(QList<WM_MSG> listMessage);//消息放入消息队列
    void pushMesageQueueHistory(WM_MSG message);//消息放入历史消息队列
    void processSync1(QList<QStringList> &sections);//处理同步函数1
    void processSync2(QList<QStringList> &sections);//处理同步函数2  处理完成标志着同步完成
    void syncistribution(QString strMessage);//根据消息判断调用哪个同步函数



    QByteArray InfotoByteArry();//同步信息转json再转bytearray
    QString getSidMappingid(const int &nsid);//得到sid对应的拼接屏设备ID号
    int  wmCompare(const WM_PROPERTY &h);//找到返回索引值 找不到返回-1
private slots:
    void slotDataSyncTimeOut();//同步定时器超时槽函数
    void slotStartMessageQueue();//启动消息队列
    void slotSendSync();//发送同步消息
    void slotOpenWinEnableTimer();
private:
    /***********网络模块*************/
    CNetWorkForClientsUdp *m_pNetWrok;//网络模块
    QString m_strIp;//拼控器Ip
    int m_nPort;//拼控器端口

    /***********同步处理*************/
    QTimer m_SyncTimer;//同步定时器
    QTimer m_SyncSendTimer;//同步定时器
    int m_nTimerMsec;//同步时间/秒
    ENM_MSG_TYPE m_enmCurrentSyncTyep;//当前的同步消息type
    QList<WM_MSG>m_listSync;//同步命令

    /***********消息队列*************/
    QTimer m_MessageQueuecTimer;//消息队列定时器
    QQueue<WM_MSG> m_MessageQueue;//控制命令消息队列
    QQueue<WM_MSG> m_MessageQueueHistory;//已发送完的消息队列
    int m_nMessageQueueCapacity;//消息队列容量
private://拼控器数据
    WM_LIST m_WMInfo;//所有窗口信息
    WM_LIST m_WMInfoCopy;//所有窗口信息
    SC_LIST m_SCInfo;//屏组信息
    QList<QString> m_listScId;//屏组真实id 需要从配置文件按顺序读取

    int m_Winnum;


    QStringList  sceneList;
    QStringList  sceneListTest;
   QSet<QString> screenSeting;

   QSet<QString>testSeting;
   QSet<QString>pingSeting;

   QMap<int,QString>ipAdressMAP;
   QMap<int,QString>sceneTabMAP;

   QStringList threeLevel;

   QMap<QString,QStringList>m_ConfigSceneTransfer;
   QMap<int,QString>m_ScreenGroupMap;


   int sidGroupTotal;
   int countJoin;
   QTimer m_openWinEnableTimer;
   int m_curSid;


   int m_bigScreenWidth;
   int m_bigScreenHeight;

   int m_sidArg;
};

#endif // SplitingXiaoNiaoNew_H
