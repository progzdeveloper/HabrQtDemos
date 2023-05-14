#pragma once
#include <QWidget>

struct WindowFunctions
{
    static WId leaderWindow(WId w);
    static bool isVisible(WId w);
    static QRect windowGeometry(WId w);
    static QImage windowImage(WId w, const QRect& rect);
    static QImage grabRootImage(const QRect &r = {});
};


class ExternalWindowHandler
    : public WindowFunctions
{
public:
    virtual ~ExternalWindowHandler() {}
    virtual bool processWindow(WId w) = 0;
};
