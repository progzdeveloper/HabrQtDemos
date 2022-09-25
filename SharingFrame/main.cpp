#include "sharingframe.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SharingFrame w;
    w.show();

    return a.exec();
}
