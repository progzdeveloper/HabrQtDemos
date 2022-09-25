#include "multilayerwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MultiLayerWindow w;
    w.show();

    return a.exec();
}
