#include "widget.h"

#include <QApplication>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QMutex>
#include "writexmlopenandwrite.h"

void outPutMessage(QtMsgType type,const QMessageLogContext &context,const QString &msg)
{
    static QMutex  mutex;
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
        text=QString("Critical:");
        break;
    case QtFatalMsg:
        text=QString("Fatal:");
        break;
    default:
        break;
    }

    QString context_info=QString("[%1 %2]").arg(context.function).arg(context.line);
    QString currentData=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString message=QString("%1%2%3%4").arg(currentData).arg(context_info).arg(text).arg(msg);

    QFile file("logAA.txt");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
       return;
    }
    QTextStream stream(&file);
    stream<< message <<"\r\n";

    file.flush();
    file.close();

    mutex.unlock();
}


int main(int argc, char *argv[])
{
    qInstallMessageHandler(outPutMessage);
    QApplication a(argc, argv);
    Widget w;
    w.show();

    WriteXmlOpenAndWrite q;
    return a.exec();
}
