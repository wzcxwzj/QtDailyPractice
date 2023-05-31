#include "widget.h"

#include <QApplication>

#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QMutex>


void outPutMsg(QtMsgType type,const QMessageLogContext &context,const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

        QString text;
        switch (type) {
        case QtDebugMsg:
            text=QString("Debug:");
        break;
        case QtWarningMsg:
            text=QString("Warning:");
        break;
        case QtCriticalMsg:
            text=QString("CritiCal:");
        break;

        case QtFatalMsg:
            text=QString("FatalMsg:");
        break;
        default:
        break;
    }
        QString cur_SaveInfo=QString("[ %1 %2 ]").arg(context.function).arg(context.line);


        QString cur_datatime=QDateTime::currentDateTime().toString("yyyy-HH-MM hh:ss:mm");
        QString msgInfo=QString("%1%2%3%4").arg(cur_datatime).arg(cur_SaveInfo).arg(text).arg(msg);

        QFile file("LOGbb.txt");
        if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            return;
        }
        QTextStream stream(&file);
        stream<< msgInfo <<"\r\n";

        file.flush();
        file.close();

        mutex.unlock();

}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(outPutMsg);
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
