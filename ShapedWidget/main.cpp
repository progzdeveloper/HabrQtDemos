#include "screenframe.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ScreenFrame w;
    w.show();

    return a.exec();
}
