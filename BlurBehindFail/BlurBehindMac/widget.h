#ifndef WIDGET_H
#define WIDGET_H
#include <QtWidgets>
#include <QWindow>
#include <QResizeEvent>

class Widget : public QWidget
{
public:
    Widget();
    ~Widget() = default;

    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void* effectsView = nullptr;
};



#endif // WIDGET_H
