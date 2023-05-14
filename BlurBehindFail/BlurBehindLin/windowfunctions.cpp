#include "windowfunctions.h"
#include "xcbwindowmanager.h"

WId WindowFunctions::leaderWindow(WId w)
{
#ifdef __linux__
    return XcbWindowManager::instance().leaderWindow(w);
#else
    return 0;
#endif
}

bool WindowFunctions::isVisible(WId w)
{
#ifdef __linux__
    return XcbWindowManager::instance().isVisible(w);
#else
    return false;
#endif
}

QRect WindowFunctions::windowGeometry(WId w)
{
#ifdef __linux__
    return XcbWindowManager::instance().geometry(w);
#else
    return QRect{};
#endif
}

QImage WindowFunctions::windowImage(WId w, const QRect& rect)
{
#ifdef __linux__
    return XcbWindowManager::instance().image(w, rect);
#else
    return QImage{};
#endif
}

QImage WindowFunctions::grabRootImage(const QRect& r)
{
#ifdef __linux__
    return XcbWindowManager::instance().grabRootImage(r);
#else
    return QImage{};
#endif
}


