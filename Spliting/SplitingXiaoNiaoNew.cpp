#include "SplitingXiaoNiaoNew.h"
#include <QApplication>
#include <QTime>
#include <QtGlobal>
#include <qrect.h>
#include <qjsondocument.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QSet>
#include <iostream>
#include <QFile>
#include <QDomDocument>
#include "CurSingleSourceChange.h"


SplitingXiaoNiaoNew::SplitingXiaoNiaoNew(QString strIp, int nPort)
{
    m_nMessageQueueCapacity = MESSAGE_QUEUE_CAPACITY;
    m_nTimerMsec = SYNC_TIMERMSEC;

    //建立连接
    m_strIp = strIp;
    m_nPort = nPort;

    m_Winnum = 0;
    m_pNetWrok = new CNetWorkForClientsUdp(this);
    m_pNetWrok->CreateConnection(strIp, nPort, this);

    m_curSid=0;

    m_listScId = CConfig::GetInstance()->GetlistScreenId();
    sidGroupTotal=m_listScId.count();  //屏組个数
    callSceneList(1, 1);

    //同步命令初始化
    InitSyncInfo();
    //消息队列启动
    connect(&m_MessageQueuecTimer, SIGNAL(timeout()), this, SLOT(slotStartMessageQueue()));
    m_MessageQueuecTimer.start(MSG_SEND_SPAKING);


    connect(&m_openWinEnableTimer, SIGNAL(timeout()), this, SLOT(slotOpenWinEnableTimer()));
   // openWinEnableTimer.start(MSG_SEND_SPAKING);

    //同步数据发送定时  当所有屏组第一条信息都过来后 100毫秒默认为所有第二条信息也处理完毕  发送同步数据
    m_SyncSendTimer.setInterval(1000);
    connect(this, SIGNAL(sigStartTimer()), &m_SyncSendTimer, SLOT(start()));
    connect(&m_SyncSendTimer, SIGNAL(timeout()), this, SLOT(slotSendSync()));
    //启动同步
    connect(&m_SyncTimer, SIGNAL(timeout()), this, SLOT(slotDataSyncTimeOut()));
    m_SyncTimer.start(m_nTimerMsec * 1000);


    readXML();
    readXMLScreenId();
//    qDebug()<<"pppppppppppppppp"<<m_ConfigSceneTransfer<<"uuuuuuuuuuuuuuuu";

    CurSingleSourceChange::GetInstance()->setIpAdressErPort(m_strIp,m_nPort,ipAdressMAP);
    connect(this, SIGNAL(testSingleSourceOpenChange(WM_LIST)), CurSingleSourceChange::GetInstance(), SLOT(singleSourceOpenChange(WM_LIST )));
    connect(this, SIGNAL(testSingleSourceMoveChange(int,int,WM_LIST )),CurSingleSourceChange::GetInstance(), SLOT(singleSourceMoveChange(int,int,WM_LIST)));

}

WM_PROPERTY SplitingXiaoNiaoNew::getWindowByPos(int sid, int x, int y)
{
    WM_PROPERTY wm;
    wm.sid = -1;
    WM_LIST wmList;
    for (int i = 0; i < m_WMInfo.size(); i++) {
        if (m_WMInfo.at(i).sid == sid)
        {
            int rx = m_WMInfo.at(i).rect.x;
            int ry = m_WMInfo.at(i).rect.y;
            int rx1 = m_WMInfo.at(i).rect.width;
            int ry1 = m_WMInfo.at(i).rect.height;
            QRect Rect(rx, ry, rx1, ry1);
            if (Rect.contains(x, y))
            {
                wmList.push_back(m_WMInfo.at(i));
            }
        }
    }
    int t = -1, f = 0;
    for (int i = 0; i < wmList.size(); i++)
    {
        if (f <= wmList.at(i).z_index) {
            f = wmList.at(i).z_index;
            t = i;
        }
    }
    if (t != -1) {
        wm = wmList.at(t);
    }
    return wm;
}

bool SplitingXiaoNiaoNew::isTopWindow(WM_PROPERTY& h)
{
    int index = wmCompare(h);
    if (index == -1)
        return false;
    int z_index = m_WMInfo.at(index).z_index;

    int tmp = 0;
    for (int i = 0; i < m_WMInfo.size(); i++) {
        tmp = qMax(m_WMInfo.at(i).z_index, tmp);
    }
    if (z_index == tmp)
        return true;
    else
        return false;
}

void SplitingXiaoNiaoNew::setSyncFrequency(int s)
{
    m_nTimerMsec = s;
    m_SyncTimer.stop();
    m_SyncTimer.start(m_nTimerMsec * 1000);
}

WM_LIST SplitingXiaoNiaoNew::findWindowByChennel(int sid, int nid)
{
    WM_LIST wmList;
    for (int i = 0; i < m_WMInfo.size(); i++) {
        if (m_WMInfo.at(i).sid == sid &&
            m_WMInfo.at(i).nid == nid)
        {
            wmList.push_back(m_WMInfo.at(i));
        }
    }
    return wmList;
}

void SplitingXiaoNiaoNew::reconnectionServer(const QString& serverIP, const int& nPort)
{
    m_pNetWrok->CreateConnection(serverIP, nPort, this);
    m_strIp = serverIP;
    m_nPort = nPort;

}
void SplitingXiaoNiaoNew::dataSync()
{
    slotDataSyncTimeOut();
}

void SplitingXiaoNiaoNew::setWindowPos(const WM_PROPERTY& h, const WM_RECT& rect) //移动窗口(h.sid,h.nid,h.wid必须有效)
{

    QList<WM_MSG>list;
    WM_MSG MSG;
    MSG.enmMsgType = TYPE_MOVE;
    MSG.nSid = 1;
    MSG.nSeqId = 1;
    WM_RECT rect1 = rect;

    float xmodulus; //x相对于65535的系数
    float ymodulus;//y相对于65535的系数
    for (int i = 0; i < m_SCInfo.size(); i++)
    {
        if (m_SCInfo.at(i).sid == h.sid)
        {
            xmodulus = m_SCInfo.at(i).xmodulus;
            ymodulus = m_SCInfo.at(i).ymodulus;
        }
    }
    int xr = rect1.x / xmodulus;
    int yr = rect1.y / ymodulus;
    int iWidth = rect1.width / xmodulus;
    int height = rect1.height / ymodulus;
    MSG.strMessage = "<UWIB,SEQ,"
        + QString::number(h.wid)
        + "&" + QString::number(xr)
        + "&" + QString::number(yr)
        + "&" + QString::number(iWidth)
        + "&" + QString::number(height) + "&"
        + QString::number(h.sid) + "," + "true" + ">";
    qDebug() << "移动窗口===============" << MSG.strMessage;

    list.push_back(MSG);
    pushMesageQueue(list);
    int nIndex = wmCompare(h);
    if (nIndex != -1) {
        m_WMInfo[nIndex].rect.x = rect1.x;
        m_WMInfo[nIndex].rect.y = rect1.y;
        m_WMInfo[nIndex].rect.width = rect1.width;
        m_WMInfo[nIndex].rect.height = rect1.height;
    }

    //同步数据
    emit dataReady(InfotoByteArry());

}

void SplitingXiaoNiaoNew::setWindowOnTop(const WM_PROPERTY& h)                //将窗口置顶(h.sid,h.wid必须有效)
{//@to,splitindex,<TorB,win_id,1>
    QList<WM_MSG>list;
    WM_MSG MSG;
    MSG.enmMsgType = TYPE_NULL;
    MSG.nSid = h.sid;
    MSG.nSeqId = h.sid;
    MSG.strMessage = "<BWOT,1,"
        + QString::number(h.wid)+","
        + QString::number(h.sid)+","
        + "true>";
    list.push_back(MSG);
    pushMesageQueue(list);
    //slotDataSyncTimeOut();

    int lowTopIndex = -1; int topIndex = -1, nIndexz = -1;
    int j = wmCompare(h);
    if(j==-1)
        return ;
    for(int i = 0 ; i<m_WMInfo.size();i++) {
        if(m_WMInfo.at(i).sid== h.sid){
            if(m_WMInfo.at(i).z_index== m_WMInfo.at(j).z_index){
                lowTopIndex = i;
            }
            if(m_WMInfo.at(i).z_index>nIndexz){
                nIndexz = m_WMInfo.at(i).z_index;
                topIndex = i;
            }
        }
    }
    if(lowTopIndex == -1||topIndex== -1||nIndexz == -1)
        return ;
    WM_PROPERTY tmp;
    tmp = m_WMInfo.at(topIndex);
    m_WMInfo[topIndex].z_index = m_WMInfo.at(lowTopIndex).z_index;
    m_WMInfo[lowTopIndex].z_index = tmp.z_index;
    emit dataReady(InfotoByteArry());

    emit testSingleSourceMoveChange(tmp.sid,tmp.z_index,m_WMInfo);

}

void SplitingXiaoNiaoNew::closeWindow(const WM_PROPERTY& h)                   //删除窗口(h.sid,h.wid必须有效)
{
    QList<WM_MSG>list;
    WM_MSG MSG;
    MSG.enmMsgType = TYPE_NULL;
    MSG.nSid = h.sid;
    MSG.nSeqId = h.sid;
    MSG.strMessage = "<SHUT,SEQ,"
        + QString::number(h.wid) + ","
        + QString::number(h.sid) + ","
        + "true"
        + ">";

    for (int i = 0; i < m_WMInfo.size(); i++) {
        if (m_WMInfo.at(i).wid == h.wid && m_WMInfo.at(i).sid == h.sid)
        {
            m_WMInfo.removeAt(i);
        }
    }

    list.push_back(MSG);
    pushMesageQueue(list);
    slotDataSyncTimeOut();


    //  emit dataReady(InfotoByteArry());


}

void SplitingXiaoNiaoNew::callScene(int sid, int n)                           //调取某组屏的第n号场景
{
    int sidOpa;
    QStringList screenNum;
    QString str=QString("%1%2").arg(QString::fromLocal8Bit("屏组")).arg(QString::number(sid));
    if(m_ConfigSceneTransfer.contains(str))
    {
        sidOpa= sid;
        screenNum=m_ConfigSceneTransfer.value(str);
    }
//    QString strScreen;
//    sceneListTest = screenSeting.toList();

//    QList<QStringList> listScreen;

//    for (int i = 0; i < sceneListTest.size(); i++)
//    {
//        QStringList listP;
//        listP = sceneListTest.at(i).split(",");
//        listScreen.push_back(listP);
//    }
//    for (int j = 0; j < listScreen.size(); j++)
//    {
//        for (int k = 0; k < listScreen.at(j).size(); k++)
//        {
//            if (listScreen.at(j).at(5) == QString::number(n))
//            {
//                strScreen = listScreen.at(j).at(3);
//                break;
//            }



//        }
//    }

    QList<WM_MSG>list;
    WM_MSG MSG2;
    MSG2.enmMsgType = TYPE_NULL;
    MSG2.nSid = sid;
    MSG2.nSeqId = sid;
    MSG2.strMessage = "<CWSN,"
        + QString::number(1) + ","
        + QString::number(sidOpa) + ","
        + screenNum.at(n-1)
        + ">";                  //<CWSN,SEQ,groupId,sceneId>


   // m_ConfigSceneTransfer.value(QString("%1%2").arg(QString::fromLocal8Bit("屏組").arg(sid))).at(n);
    list.push_back(MSG2);
    pushMesageQueue(list);
    slotDataSyncTimeOut();

qDebug()<<"sceneeeeeeeeeeeeeeeeee"<<MSG2.strMessage;
//    sceneList.clear();
//    sceneListTest.clear();
//    screenSeting.clear();
    //cleanGropWindow(sid);

//  for (int i = 0; i < m_WMInfo.size(); i++) {
//        if (m_WMInfo.at(i).sid == sid)
//        {
//            m_WMInfo.removeAt(i);
//        }
//}
   // }

//    m_WMInfo.clear();
//    emit dataReady(InfotoByteArry());

}

void SplitingXiaoNiaoNew::openWindow(int sid, int nid, const WM_RECT& rect)   //将某一信号通道开窗到某组屏的具体某个矩形区域
{

    m_curSid=sid;

    if(m_openWinEnableTimer.isActive())
    {
        m_openWinEnableTimer.stop();
        m_openWinEnableTimer.start(200);
    }else {
        m_openWinEnableTimer.start(200);
    }

qDebug()<<"11111111111111 openWindow  : "<<sid<<nid<<rect.x<<rect.y<<rect.width<<rect.height;
    QList<WM_MSG>list;
    WM_MSG MSG;
    MSG.enmMsgType = TYPE_NULL;
    MSG.nSid = sid;
    MSG.nSeqId = sid;
    float xmodulus; //x相对于65535的系数
    float ymodulus;//y相对于65535的系数
    for (int i = 0; i < m_SCInfo.size(); i++)
    {
        if (m_SCInfo.at(i).sid == sid)
        {
            xmodulus = m_SCInfo.at(i).xmodulus;
            ymodulus = m_SCInfo.at(i).ymodulus;
        }
    }

    int iwid = 0;
    for (int i = 0; i < m_WMInfo.size(); i++) {
        if (m_WMInfo.at(i).sid == sid)
        {
            iwid = qMax(iwid, m_WMInfo.at(i).wid);

             int x=rect.x-200;
             int y=rect.y-200;
             int width=rect.width+200;
             int height=rect.height+200;

             QRect rectInfo(x,y,width,height);

             WM_PROPERTY hWin= m_WMInfo.at(i);
             WM_RECT winRect=hWin.rect;
             QRect winCRect(winRect.x,winRect.y,winRect.width,winRect.height);

             if(rectInfo.contains(winCRect))
             {
               closeWindow(hWin);
             }
        }
    }
    iwid = iwid + 1;
    int xr = rect.x / xmodulus;
    int yr = rect.y / ymodulus;
    int iWidth = rect.width / xmodulus;
    int height = rect.height / ymodulus;


 qDebug()<<"openWindo  sid xmodulus ymodulus rect.x rect.y rect.width rect.height-------  : "<<sid<<xmodulus<<ymodulus<<rect.x<<rect.y<<rect.width<<rect.height;
    MSG.strMessage = "<OPEN,"
        + QString::number(1) + ","
        + QString::number(iwid)
        + "&"
        + ipAdressMAP.value(nid)  //IP
        + "&"
        + QString::number(xr)
        + "&"
        + QString::number(yr)
        + "&"
        + QString::number(iWidth)
        + "&"
        + QString::number(height)
        + "&"
        + QString::number(sid) + ","
        + "false" + ">";    //true

    list.push_back(MSG);
    pushMesageQueue(list);


     qDebug()<<"openWind+++++nid: "<<nid<<" ipAdressMAP: "<<ipAdressMAP.value(nid)<<" strMessage: "<<MSG.strMessage;
     int end = 0;
     for(int i = 0 ; i<m_WMInfo.size();i++){
         if(m_WMInfo.at(i).sid== sid){
             end = i;
         }
     }
     if(end!=0){
         WM_PROPERTY w = m_WMInfo.at(end);
         w.rect = rect;
         w.nid = nid;
         w.wid = iwid;
         w.z_index = w.z_index+1;
         m_WMInfo.insert(m_WMInfo.begin()+end+1,w);
     }
     else{
         WM_PROPERTY w;
        for (int i = 0; i < m_SCInfo.size(); i++) {
             if (m_SCInfo.at(i).sid == sid) {
                 w.cols = m_SCInfo.at(i).cols;
                 w.rows = m_SCInfo.at(i).rows;
                 w.sid = sid;
                 w.nid =ipAdressMAP.key(ipAdressMAP.value(nid));
                 w.sh = height + yr;
                 w.sw = iWidth + xr;
                 w.wid = iwid;
                 w.z_index = 1;
                 w.rect = rect;
                    m_WMInfo.push_back(w);

                    w.cols = m_SCInfo.at(i).cols;
                    w.rows = m_SCInfo.at(i).rows;
                    w.sid = sid;
                    w.nid =ipAdressMAP.key(ipAdressMAP.value(nid));
                    w.sh = height + yr;
                    w.sw = iWidth + xr;
                    w.wid = iwid;
                    w.z_index = 1;
                    m_WMInfoCopy.push_back(w);
                }
        }
     }
        emit dataReady(InfotoByteArry());

        slotDataSyncTimeOut();
//        int delay = 500;
//        if(sid ==1){
//            delay = 2000;
//        }
//        delay_msec(delay);

        emit testSingleSourceOpenChange(m_WMInfo);
}
void SplitingXiaoNiaoNew::openWindowTest(int sid,int iwid,QString nid,int xr,int yr,int iWidth,int height)
{
    m_curSid=sid;

    if(m_openWinEnableTimer.isActive())
    {
        m_openWinEnableTimer.stop();
        m_openWinEnableTimer.start(50);
    }else {
        m_openWinEnableTimer.start(50);
    }



    QList<WM_MSG>list;
    WM_MSG MSG;
    MSG.enmMsgType = TYPE_NULL;
    MSG.nSid = sid;
    MSG.nSeqId = sid;
    MSG.strMessage = "<OPEN,"
        + QString::number(1) + ","
        + QString::number(iwid)
        + "&"
        + nid  //IP
        + "&"
        + QString::number(xr)
        + "&"
        + QString::number(yr)
        + "&"
        + QString::number(iWidth)
        + "&"
        + QString::number(height)
        + "&"
        + QString::number(sid) + ","
        + "false" + ">";    //true
    qDebug()<<"openWindowTestopenWindowTest++++nid: "<<nid<<" strMessage: "<<MSG.strMessage;

    list.push_back(MSG);
    pushMesageQueue(list);
}
void SplitingXiaoNiaoNew::cleanAllWindow()                                        //删除所有屏窗口
{

    if(m_openWinEnableTimer.isActive())
    {
        m_openWinEnableTimer.stop();
        m_openWinEnableTimer.start(100);
    }else {
        m_openWinEnableTimer.start(100);
    }

    int sid = 1;
    QList<WM_MSG>list;
    WM_MSG MSG;
    MSG.enmMsgType = TYPE_NULL;
    MSG.nSid = sid;
    MSG.nSeqId = sid;
    MSG.strMessage = "<SALL,SEQ,"
        + QString::number(sid) + ","
        + "false" + ">";

    list.push_back(MSG);
    pushMesageQueue(list);
    slotDataSyncTimeOut();

    m_WMInfo.clear();
    emit dataReady(InfotoByteArry());


}

void SplitingXiaoNiaoNew::cleanGropWindow(int sid)
{
//    sid = 1;
    if(m_openWinEnableTimer.isActive())
    {
        m_openWinEnableTimer.stop();
        m_openWinEnableTimer.start(120);
    }else {
        m_openWinEnableTimer.start(120);
    }

    QList<WM_MSG>list;
    WM_MSG MSG;
    MSG.enmMsgType = TYPE_NULL;
    MSG.nSid = sid;
    MSG.nSeqId = sid;
    MSG.strMessage = "<SALL,SEQ,"
        + QString::number(sid) + ","
        + "true" + ">";

    list.push_back(MSG);
    pushMesageQueue(list);
    slotDataSyncTimeOut();

    m_WMInfo.clear();
    emit dataReady(InfotoByteArry());


}

void SplitingXiaoNiaoNew::dataArrived(QByteArray arrMessage, QString strIp, int nPort)
{
    qDebug() << arrMessage << "2222222222222222" << arrMessage;
    QString strMsg = QString(arrMessage);


    if (strMsg.contains("GWSL") && !strMsg.contains("OK") && strMsg != "")
    {
        sceneList << strMsg.split(QRegExp("[<>]"));
        sceneList.removeAll(QString(""));
        screenSeting = sceneList.toSet();
    }


    QString strMessage = QString(arrMessage);
    syncistribution(arrMessage);
}

void SplitingXiaoNiaoNew::connectStateChanged(bool bState, QString strIp, int nPort)
{
    if (!bState && (strIp.compare(m_strIp) == 0)) {
        emit deviceError();
        qDebug() << "拼控器掉线IP:" << strIp;
    }
}



void SplitingXiaoNiaoNew::pushMesageQueue(QList<WM_MSG> listMessage)
{
    if (m_MessageQueue.size() + listMessage.size() < m_nMessageQueueCapacity) {
        for (int i = 0; i < listMessage.size(); i++) {
            m_MessageQueue.push_back(listMessage.at(i));
        }
    }
}

void SplitingXiaoNiaoNew::pushMesageQueueHistory(WM_MSG Message)
{
    if (m_MessageQueueHistory.size() > MESSAGE_QUEUE_CAPACITY) {
        m_MessageQueueHistory.pop_front();
    }
    m_MessageQueueHistory.push_back(Message);
}
void SplitingXiaoNiaoNew::setMessageQueueCapacity(int nCapacity)
{
    m_nMessageQueueCapacity = nCapacity;
}

void SplitingXiaoNiaoNew::openWindow1(int sid, int nid, QList<WM_PROPERTY> listRect)
{

        for(int i=0;i<m_WMInfoCopy.size();i++)
         {
            QList<WM_MSG>list;
            WM_MSG MSG;
            MSG.enmMsgType = TYPE_NULL;
            MSG.nSid = 1;
            MSG.nSeqId = 1;
            float xmodulus; //x相对于65535的系数
            float ymodulus;//y相对于65535的系数
            for (int i = 0; i < m_SCInfo.size(); i++)
            {
                if (m_SCInfo.at(i).sid == 1)
                {
                    xmodulus = m_SCInfo.at(i).xmodulus;
                    ymodulus = m_SCInfo.at(i).ymodulus;
                }
            }
            int iwid = -1;
            for (int i = 0; i < m_WMInfoCopy.size(); i++) {
                if (m_WMInfoCopy.at(i).sid == sid)
                {
                    iwid = qMax(iwid, m_WMInfoCopy.at(i).wid);
                }
            }
            iwid = iwid + 1;
            int xr = listRect.at(i).rect.x / xmodulus;
            int yr = listRect.at(i).rect.y / ymodulus;
            int iWidth =  listRect.at(i).rect.width / xmodulus;
            int height =  listRect.at(i).rect.height / ymodulus;

            MSG.strMessage = "<OPEN,"
                + QString::number(1) + ","
                + QString::number(iwid)
                + "&"
                + ipAdressMAP.value(nid)  //IP
                + "&"
                + QString::number(xr)
                + "&"
                + QString::number(yr)
                + "&"
                + QString::number(iWidth)
                + "&"
                + QString::number(height)
                + "&"
                + QString::number(sid) + ","
                + "true" + ">";

            list.push_back(MSG);
            pushMesageQueue(list);
        }

}

void SplitingXiaoNiaoNew::callSceneList(int sid, int n)
{
    QList<WM_MSG>list2;
    WM_MSG MSG;
    MSG.enmMsgType = TYPE_NULL;
    MSG.nSid = sid;
    MSG.nSeqId = sid;
    MSG.strMessage = "<GWSL,"
        + QString::number(sid) + ","
        + QString::number(sid)
        + ">";
    list2.push_back(MSG);
    pushMesageQueue(list2);

    slotDataSyncTimeOut();
}

void SplitingXiaoNiaoNew::readXML()
{
        QString strXmlPath = QCoreApplication::applicationDirPath();
        strXmlPath += "/XiaoNiaoConfig.xml";

      //  qDebug() << "xmlPath:" << strXmlPath << endl;
        QFile file(strXmlPath);
        if(!file.open(QFile::ReadOnly))
        {
            qDebug()<<"open file failed";
            return;
        }

       //DOM关联文件操作
        QDomDocument doc;
        if(!doc.setContent(&file))
        {
           file.close();
            return;
        }
        file.close();

        QDomElement root=doc.documentElement();
        //获取第一个节点
         QDomNode node=root.previousSibling();


         //读
         while (!node.isNull())
         {
                QDomElement element=node.toElement();
             //修改节点
             //遍历第一个节点的子节点
                 QDomNodeList list1=element.childNodes();

                 for(int i=0;i<list1.count();++i)
                  {
                      QDomNode n=list1.at(i);
                      QString tagName = n.toElement().tagName(); //获取节点的元素名称
//qDebug() <<"tagNametagNametagNametagName000" << tagName;
                      if (tagName.contains("Ip"))         //节点标记查找,名称为"name"的节点
                      {
                            ipAdressMAP[i+1]=n.toElement().text();
                      }
                 }
              //下一个兄弟节点
             node=node.nextSibling();
      }

}

void SplitingXiaoNiaoNew::readXMLScreenId()
{
    QString strXmlPath = QCoreApplication::applicationDirPath();
    strXmlPath += "/XiaoNiaoScrrenidConfig.xml";

    //qDebug() << "xmlPath:" << strXmlPath << endl;
    QFile file(strXmlPath);
    if(!file.open(QFile::ReadOnly))
    {
        qDebug()<<"open file failed";
        return;
    }

   //DOM关联文件操作
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
       file.close();
        return;
    }
    file.close();

    QDomElement root=doc.documentElement();
//    //获取第一个节点
//     QDomNode node=root.previousSibling();
//     //读
//     while (!node.isNull())
//     {
//            QDomElement element=node.toElement();
//         //修改节点
//         //遍历第一个节点的子节点
//            QDomNodeList list1=element.childNodes();
//             for(int i=0;i<list1.count();++i)
//              {
//                  QDomNode n=list1.at(i);
//                  QString tagName = n.toElement().tagName(); //获取节点的元素名称

//                  QString sid=QString(("%1%2").arg("SID").arg(QString::number(i)));
//                  if(tagName.contains(sid))
//                  {
//                      //sceneTabMAP[i]=n.toElement().text();
//                        int k;
//                        k=i;
//                  }
//                  if(tagName.contains("screenID"))
//                  {
//                      sceneTabMAP[i]=n.toElement().text();

//                  }
//             }
//          //下一个兄弟节点
//         node=node.nextSibling();
//    }

     for(int i = 0 ; i<root.childNodes().size();i++)
     {
         QList<QString>list;
         QDomElement Node = root.childNodes().at(i).toElement();
         QString sid;
         for(int j = 0 ;j<Node.childNodes().size(); j++)
         {
//             if(Node.childNodes().at(j).toElement().tagName()== "Id"){
//                 sid =  Node.childNodes().at(j).toElement().text();
//                 continue;
//             }

             list.push_back(Node.childNodes().at(j).toElement().text());

         }
         m_ConfigSceneTransfer[root.childNodes().at(i).toElement().tagName()] = list;


        // m_ConfigUpScreen[root.childNodes().at(i).toElement().tagName()] = sid;
     }

}

QString SplitingXiaoNiaoNew::getSidMappingid(const int& nsid)
{
    if (nsid > 0 && nsid - 1 < m_listScId.size()) {
        return m_listScId.at(nsid - 1);
    }
    else
        return "";
}

int SplitingXiaoNiaoNew::wmCompare(const WM_PROPERTY& h)
{
    int nIndex = -1;
    for (int i = 0; i < m_WMInfo.size(); i++) {
        if (m_WMInfo.at(i).sid == h.sid && m_WMInfo.at(i).wid == h.wid) {
            nIndex = i;
            break;
        }
    }
    return nIndex;
}

void SplitingXiaoNiaoNew::InitSyncInfo()
{
    countJoin=1;

    QList<WM_MSG> list;
    WM_MSG MSG1;
    MSG1.enmMsgType = TYPE_SYNC1;
    MSG1.nSeqId = 1;
    MSG1.strMessage = "<GVWS,SEQ>";     //屏組信息
    MSG1.nSid = 1;
    list.push_back(MSG1);
    pushMesageQueue(list);

    for (int var = 0; var < m_listScId.size(); var++) {
        QList<WM_MSG> list1;
        WM_MSG MSG2;
        MSG2.enmMsgType = TYPE_SYNC2;
        MSG2.nSeqId = m_listScId.at(var).toInt();
        MSG2.strMessage = "<GAWD,SEQ,"+m_listScId.at(var)+">";   //窗口信息
        MSG2.nSid = m_listScId.at(var).toInt();
        list1.push_back(MSG2);


//        WM_MSG MSG3;                        //调场景
//        MSG3.enmMsgType = TYPE_NULL;
//        MSG3.nSid =  m_listScId.at(var);
//        MSG3.nSeqId =  m_listScId.at(var);
//        MSG3.strMessage = "<GWSL,"
//            + m_listScId.at(var) + ","
//            + m_listScId.at(var)
//            + ">";
//        list.push_back(MSG3);

       pushMesageQueue(list1);
    }

}
void SplitingXiaoNiaoNew::syncistribution(QString strMessage)
{
   qDebug() << "@@@@@@@@@" << strMessage << "############";

    QStringList list = strMessage.split(QRegExp("[<>]"), QString::SplitBehavior::SkipEmptyParts); //把每一个块装进一个QStringList中;
    //qDebug() << "syncistribution list count:" << list.count();
    QStringList sections;
    for (int i = 0; i < list.size(); i++) {
        if (list.at(i) != "")
            sections.push_back(list.at(i));
    }
    QList<QStringList>sections2;
    QList<QStringList>sections3;
    QList<QStringList>sections4;


    for (int i = 0; i < sections.size(); i++)
    {
        QStringList str = sections.at(i).split(QRegExp("\n"));
        if (str.at(0) != "") {
            sections2.push_back(str);
        }
        if (sections2.isEmpty()) {
            return;
        }
    }

    for (int i = 0; i < sections2.size(); i++)
    {
        QStringList strList = sections2.at(i).at(0).split(",", QString::SkipEmptyParts);
        QString scuStr;

        for (int j = 0; j < strList.size(); j++)
        {
            scuStr += strList.at(j) + ",";

        }
        if (scuStr.contains("GVWS")&& !scuStr.contains("OK"))
        {
                pingSeting<< scuStr;
        }

        if(scuStr.contains("GVWS")&& scuStr.contains("OK"))
         {

            QStringList listSet1=pingSeting.toList();
            for(int l=0;l<listSet1.size();l++)
            {
             QStringList listL=listSet1.at(l).split(",");
             sections3.push_back(listL);
            }
             processSync1(sections3);
             pingSeting.clear();
        }

        if (scuStr.contains("GAWD"))
        {
            if (!scuStr.contains("OK"))
            {
                testSeting << scuStr;
            }
            else {/*sidGroupTotal=1;*/
                if(countJoin ==  sidGroupTotal) //countJoin 每次同步赋初值 1
                {
                    QStringList ptr = testSeting.toList();
                    for (int i = 0; i < ptr.size(); i++)
                    {
                        QStringList straa = ptr.at(i).split(",");
                        sections4.push_back(straa);
                    }
                    if (!sections4.isEmpty())
                    {
                        processSync2(sections4);
                        testSeting.clear();
                    }
                }

                countJoin++;
            }

        }

    }

}
void SplitingXiaoNiaoNew::processSync2(QList<QStringList>& sections)
{

    m_WMInfo.clear();
    for (int i = 0; i < sections.size(); i++)
    {
        int nsid =sections.at(i).at(3).toInt();

        for (int j = 0; j < m_SCInfo.size(); j++) {
            if (m_SCInfo.at(j).sid == nsid) {

                SC_PROPERTY screen;
                memset(&screen, 0, sizeof(SC_PROPERTY));
                screen=m_SCInfo.at(j);


                WM_PROPERTY stWM;
                stWM.rows = screen.rows;     //行数
                stWM.cols = screen.cols;    //列数
                stWM.sw = screen.sw;      //水平宽度 像素
                stWM.sh = screen.sh;     //垂直高度 像素
                stWM.sid = screen.sid;

                stWM.wid = sections.at(i).at(4).toInt(); //wid为组id*1000 +窗口Id

                stWM.nid = ipAdressMAP.key(sections.at(i).at(5));//sections.at(i).at(1).toInt();

                stWM.z_index = sections.at(i).at(10).toInt();

                int x = sections.at(i).at(6).toInt();
                int y = sections.at(i).at(7).toInt();
                int width = sections.at(i).at(8).toInt();
                int height = sections.at(i).at(9).toInt();

                stWM.rect.x = x * screen.xmodulus;
                stWM.rect.y = y * screen.ymodulus;
                stWM.rect.width = width * screen.xmodulus;
                stWM.rect.height = height * screen.ymodulus;
                m_WMInfo.push_back(stWM);
                qDebug() << "Sync2处理结果 wid nid index rectx " << stWM.wid << stWM.nid << stWM.z_index << stWM.rect.x << stWM.rect.y << stWM.rect.width << stWM.rect.height << "sid:" << stWM.sid;

            }
        }
    }
   // qDebug() << "-----m_WMInfo.size()--------- " << m_WMInfo.size();

      emit dataReady(InfotoByteArry());



    if(m_WMInfo.size()!=0)
        CurSingleSourceChange::GetInstance()->setSyncWMLIST(m_WMInfo);

}

void SplitingXiaoNiaoNew::processSync1(QList<QStringList>& sections)
{
    m_SCInfo.clear();
    for (int d = 0; d < sections.size(); d++)
    {

        int rows = 0, cols = 0, sw = 0, sh = 0, sid = sections.at(d).at(3).toInt();
        sw = sections.at(d).at(8).toInt();
        sh = sections.at(d).at(9).toInt();
        rows = sections.at(d).at(10).toInt();
        cols = sections.at(d).at(11).toInt();

        SC_PROPERTY screen;
        screen.sid =sid;
        screen.cols = cols;     //行数
        screen.rows = rows;     //列数
        screen.sw = sw * cols;       //水平宽度 像素
        screen.sh = sh * rows;       //垂直高度 像素

//        if(sid == 1)
//        {
//           screen.sw=13056;
//           screen.sh=2808;
//        }

        screen.xmodulus = 65535.00 / screen.sw;
        screen.ymodulus = 65535.00 / screen.sh;
        m_SCInfo.push_back(screen);
      qDebug()<<"++++++++SIDddddddd-- size, sid,xmodulus,ymodulus,sw,sh, -------"<<m_SCInfo.size()<< m_SCInfo.at(d).sid<< m_SCInfo.at(d).xmodulus<<m_SCInfo.at(d).ymodulus<< m_SCInfo.at(d).sw <<m_SCInfo.at(d).sh;

    }
}


QByteArray SplitingXiaoNiaoNew::InfotoByteArry()
{
    QJsonObject jJsonObj;
    QJsonArray jJsonArray;

    for (int i = 0; i < m_WMInfo.size(); i++)
    {
        QJsonObject wm;
        wm.insert(QString("cols"), m_WMInfo.at(i).cols);
        wm.insert(QString("nid"), m_WMInfo.at(i).nid);

        QJsonObject rect;
        rect.insert(QString("height"), m_WMInfo.at(i).rect.height);
        rect.insert(QString("width"), m_WMInfo.at(i).rect.width);
        rect.insert(QString("x"), m_WMInfo.at(i).rect.x);
        rect.insert(QString("y"), m_WMInfo.at(i).rect.y);
        wm.insert(QString("rect"), rect);

        wm.insert(QString("rows"), m_WMInfo.at(i).rows);
        wm.insert(QString("sh"), m_WMInfo.at(i).sh);
        wm.insert(QString("sw"), m_WMInfo.at(i).sw);
        wm.insert(QString("sid"), m_WMInfo.at(i).sid);
        wm.insert(QString("nid"), m_WMInfo.at(i).nid);
        wm.insert(QString("wid"), m_WMInfo.at(i).wid);
        wm.insert(QString("z"), m_WMInfo.at(i).z_index);
        jJsonArray.insert(i, wm);
    }

    jJsonObj.insert(QString("results"), jJsonArray);
    qDebug() << jJsonObj;
    QJsonDocument document;
    document.setObject(jJsonObj);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    return byte_array;
}
void SplitingXiaoNiaoNew::slotDataSyncTimeOut()
{
    InitSyncInfo();
}

void SplitingXiaoNiaoNew::slotStartMessageQueue()
{
    //循环处理消息
    if (m_MessageQueue.size() != 0) {
        if (!m_MessageQueue.isEmpty()) {
            QString strorder = m_MessageQueue.first().strMessage;
            m_enmCurrentSyncTyep = m_MessageQueue.first().enmMsgType;
            pushMesageQueueHistory(m_MessageQueue.first());
            m_MessageQueue.pop_front();
            m_pNetWrok->Send(m_strIp, m_nPort, strorder);
            qDebug() << strorder << "111111111111";
        }
    }

}

void SplitingXiaoNiaoNew::slotSendSync()
{
    //  emit dataReady(InfotoByteArry());
    m_SyncSendTimer.stop();
}
void SplitingXiaoNiaoNew::delay_msec(unsigned int msec)
{
    //定义一个新的事件循环
    QEventLoop loop;
    //创建单次定时器,槽函数为事件循环的退出函数
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    //事件循环开始执行,程序会卡在这里,直到定时时间到,本循环被退出
    loop.exec();
}
void SplitingXiaoNiaoNew::slotOpenWinEnableTimer()
{
//    for (int var = 0; var < m_listScId.size(); var++) {

//        if(m_curSid != m_listScId.at(var))
//            break;
        QList<WM_MSG> list1;
        WM_MSG MSG2;
        MSG2.enmMsgType = TYPE_SYNC1;
        MSG2.nSeqId = m_curSid;
        MSG2.strMessage = "<ENAB,SEQ,"+QString::number(m_curSid)+">";   //窗口信息
        MSG2.nSid = m_curSid;
        list1.push_back(MSG2);
        pushMesageQueue(list1);

        qDebug() << "ENABENABENAB111111111111"<<MSG2.strMessage;
m_openWinEnableTimer.stop();
//    }

}
