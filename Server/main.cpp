#include "myserver.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyServer w;

    a.exec();
    return 0;
}
