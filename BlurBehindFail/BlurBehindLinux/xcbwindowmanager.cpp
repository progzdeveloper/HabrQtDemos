#include "xcbwindowmanager.h"
#include "windowfunctions.h"
#include <QRect>
#include <QImage>
#ifdef __linux__
// XCB headers
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcbext.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_ewmh.h>

// X11 headers must be the last
// to avoid name clashing
#include <X11/Xlib.h>
#include <X11/keysym.h>

XcbWindowManager::XcbWindowManager()
{
    // Get XCB connection with a little help from Qt
    mConn_ = QX11Info::connection();
    if (mConn_ == nullptr)
    {
        qCritical("xcb connection failed");
        return;
    }
}

XcbWindowManager &XcbWindowManager::instance()
{
    static XcbWindowManager gInst;
    return gInst;
}

xcb_connection_t *XcbWindowManager::xcb() const
{
    return mConn_;
}

xcb_window_t XcbWindowManager::leaderWindow(xcb_window_t _w)
{
    for (;;)
    {
        xcb_query_tree_cookie_t cookie = xcb_query_tree(mConn_, _w);
        cmem_ptr<xcb_query_tree_reply_t> reply(xcb_query_tree_reply(mConn_, cookie, NULL));
        if (!reply)
            break;

        if (reply->parent == XCB_WINDOW_NONE || reply->parent == reply->root)
            break;

        _w = reply->parent;
    }
    return _w;
}

bool XcbWindowManager::isVisible(xcb_window_t _w)
{
    if (!mConn_)
        return false;

    cmem_ptr<xcb_get_window_attributes_reply_t> attribs(xcb_get_window_attributes_reply(mConn_, xcb_get_window_attributes(mConn_, _w), nullptr));
    if (!attribs)
        return false;

    return attribs->_class == XCB_WINDOW_CLASS_INPUT_OUTPUT &&
           attribs->map_state == XCB_MAP_STATE_VIEWABLE;// &&
           /*!attribs->override_redirect;*/
}

QRect XcbWindowManager::geometry(xcb_window_t _w) const
{
    if (!mConn_)
        return QRect{};

    QRect windowRect;
    cmem_ptr<xcb_get_geometry_reply_t> geometry(xcb_get_geometry_reply(mConn_, xcb_get_geometry(mConn_, _w), nullptr));
    if (geometry)
        windowRect.setRect(geometry->x, geometry->y, geometry->width, geometry->height);
    return windowRect;
}


QImage XcbWindowManager::image(xcb_window_t _w, const QRect &rect) const
{
    auto img_cookie = xcb_get_image(
        mConn_, XCB_IMAGE_FORMAT_Z_PIXMAP,
        _w, rect.x(), rect.y(), rect.width(), rect.height(), static_cast<uint32_t>(~0)
    );
    cmem_ptr<xcb_get_image_reply_t> xcbimg(xcb_get_image_reply(mConn_,img_cookie, nullptr));
    if (xcbimg)
    {
        const uchar* imgData = xcb_get_image_data(xcbimg.get());
        QImage image(imgData, rect.width(), rect.height(), QImage::Format_ARGB32);
        image.detach();
        return image;
    }
    return QImage{};
}

QImage XcbWindowManager::grabRootImage(const QRect &r) const
{
    if (!mConn_)
        return QImage{};

    xcb_window_t root = xcb_setup_roots_iterator(xcb_get_setup(mConn_)).data->root;
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(mConn_, 0, strlen("_XROOTPMAP_ID"), "_XROOTPMAP_ID");
    cmem_ptr<xcb_intern_atom_reply_t> reply(xcb_intern_atom_reply(mConn_, cookie, 0));
    if (!reply)
        return QImage{};

     const xcb_atom_t atom = reply->atom;
     xcb_get_property_cookie_t image_cookie = xcb_get_property(mConn_, 0, root, atom, XCB_ATOM_PIXMAP, 0, 1);
     cmem_ptr<xcb_get_property_reply_t> image_reply(xcb_get_property_reply(mConn_, image_cookie, nullptr));
     if (!image_reply)
         return QImage{};

     if (xcb_get_property_value_length(image_reply.get()) == 0)
         return QImage{};

     const QRect area = r.isValid() ? r : geometry(root);

     xcb_drawable_t drawable = *(xcb_drawable_t*)xcb_get_property_value(image_reply.get());
     auto img_cookie = xcb_get_image(
         mConn_, XCB_IMAGE_FORMAT_Z_PIXMAP,
         drawable, area.x(), area.y(), area.width(), area.height(), static_cast<uint32_t>(~0)
     );
     cmem_ptr<xcb_get_image_reply_t> xcbimg(xcb_get_image_reply(mConn_, img_cookie, nullptr));
     if (xcbimg)
     {
         const uchar* imgData = xcb_get_image_data(xcbimg.get());
         QImage image(imgData, area.width(), area.height(), QImage::Format_RGB32);
         image.detach();
         return image;
     }
     return QImage{};
}

void XcbWindowManager::traverse(ExternalWindowHandler *handler)
{
    xcb_window_t root = xcb_setup_roots_iterator(xcb_get_setup(mConn_)).data->root;

    xcb_window_t* children;

    /* Get the tree of windows whose parent is root window (= all) */
    cmem_ptr<xcb_query_tree_reply_t> reply(xcb_query_tree_reply(mConn_, xcb_query_tree(mConn_, root), nullptr));
    if (!reply)
        return;

    /* Request the children count */
    const int len = xcb_query_tree_children_length(reply.get());
    if (len < 1)
        return;

    /* Request the window tree */
    children = xcb_query_tree_children(reply.get());
    if (!children)
        return;

    for (int i = 0; i < len; ++i)
    {
        auto w = children[i];
        if (processWindow(w, handler))
            break;
    }
}

bool XcbWindowManager::processWindow(xcb_window_t w, ExternalWindowHandler *handler)
{
    return (handler && handler->processWindow(w));
}
#endif
