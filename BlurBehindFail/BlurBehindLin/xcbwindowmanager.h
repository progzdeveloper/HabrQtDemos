#pragma once
#ifdef __linux__

#include <memory>

#include <QObject>
#include <QRect>
#include <QtX11Extras/QX11Info>

// XCB headers
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>

class ExternalWindowHandler;


class XcbWindowManager
{
    Q_DISABLE_COPY(XcbWindowManager)

    XcbWindowManager();

public:
    struct cdeleter
    {
        void operator()(void* _ptr) const { free(_ptr); }
    };

    template<class T>
    using cmem_ptr = std::unique_ptr<T, cdeleter>;

    static XcbWindowManager& instance();

    xcb_connection_t* xcb() const;

    xcb_window_t leaderWindow(xcb_window_t _w);
    bool isVisible(xcb_window_t _w);
    QRect geometry(xcb_window_t _w) const;
    QImage image(xcb_window_t _w, const QRect& rect) const;
    QImage grabRootImage(const QRect& r = {}) const;

    void traverse(ExternalWindowHandler* handler);

private:
    bool processWindow(xcb_window_t w, ExternalWindowHandler *handler);

private:
    xcb_connection_t* mConn_;
};

#endif
