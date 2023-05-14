#include <QApplication>
#include <widget.h>

// stub code
#ifndef __APPLE__
Widget::Widget() : QWidget(nullptr) {}
void Widget::moveEvent(QMoveEvent *event) { QWidget::moveEvent(event); }
void Widget::resizeEvent(QResizeEvent *event) { QWidget::resizeEvent(event); }
#endif

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Widget window;
    window.resize(300, 300);
    window.show();

    return app.exec();
}
