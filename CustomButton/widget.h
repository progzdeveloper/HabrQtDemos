#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class CustomButton;
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

private Q_SLOTS:
    void selectColor();
    void setupCounter();
    void resetBagde();

protected:
    void paintEvent(QPaintEvent*);

private:
    CustomButton* button;
};

#endif // WIDGET_H
