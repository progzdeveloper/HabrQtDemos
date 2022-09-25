#include "toolwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ToolWindow w;
    w.show();

    return a.exec();
}
