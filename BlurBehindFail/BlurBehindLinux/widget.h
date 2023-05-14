#pragma once
#include <QWidget>

class WindowCompositionHandler;
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent *event);
    void hideEvent(QHideEvent* event);
    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);

private Q_SLOTS:
    void compose();

private:
    QPixmap pixmap;
    QPoint position;
    QTimer* timer;
    WindowCompositionHandler* compositor;
};
