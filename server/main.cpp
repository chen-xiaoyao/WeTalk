#include "qqserver.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    //正经代码
    QApplication a(argc, argv);
    QQServer w;
    w.show();
    return a.exec();
}
