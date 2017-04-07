#include "admin.h"
#include "start.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Start *startWindow = new Start();
    startWindow->show();
    a.exec();

    delete startWindow;
    return 0;
}
