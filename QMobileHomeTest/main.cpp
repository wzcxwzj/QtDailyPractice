#include "widget.h"

#include <QApplication>
#include "ccwindowwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CCWindowWidget w;
    w.show();
    return a.exec();
}
