#ifndef SCREENFRAME_H
#define SCREENFRAME_H

#include "shapedwidget.h"

class QPushButton;
class ScreenFrame : public ShapedWidget
{
public:
    explicit ScreenFrame(QScreen* _screen = Q_NULLPTR);

private:
    QPushButton* closeButton_;
};

#endif // SCREENFRAME_H
