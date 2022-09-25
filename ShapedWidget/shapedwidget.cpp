#include "shapedwidget.h"
#include <QEvent>

ShapedWidget::ShapedWidget(QWidget *_parent, Qt::WindowFlags _flags)
    : QFrame(_parent, _flags)
{
}

void ShapedWidget::updateMask()
{
    setMask(regionMask());
}

void ShapedWidget::resizeEvent(QResizeEvent *_event)
{
    updateMask();
    QFrame::resizeEvent(_event);
}

void ShapedWidget::childEvent(QChildEvent *_event)
{
    QObject* object = _event->child();
    if (_event->added() && object->isWidgetType() && object->parent() == this)
    {
        // reset event filter
        object->removeEventFilter(this);
        object->installEventFilter(this);
    }
}

bool ShapedWidget::eventFilter(QObject *_watched, QEvent *_event)
{
    if (_watched->isWidgetType() && _watched->parent() == this)
    {
        QWidget* widget = static_cast<QWidget*>(_watched);
        switch(_event->type())
        {
        case QEvent::ParentChange:
            if (!isAncestorOf(widget))
            {
                widget->removeEventFilter(this);
                removeRegion(widget, mask());
            }
            break;
        case QEvent::Show:
            setMask(appendRegion(widget, mask()));
            break;
        case QEvent::Hide:
            setMask(removeRegion(widget, mask()));
            break;
        case QEvent::Move:
        case QEvent::Resize:
            _watched->event(_event);
            setMask(regionMask());
            return true;
        default:
            break;
        }
    }
    return QWidget::eventFilter(_watched, _event);
}

QRegion ShapedWidget::regionMask() const
{
    const int w = frameWidth();
    QRegion region(rect());
    region -= rect().adjusted(w, w, -w, -w);

    // traverse all direct children
    const QList<QWidget*> childrenList = findChildren<QWidget*>(QString{}, Qt::FindDirectChildrenOnly);
    for (const auto w : childrenList)
    {
        if (!w->isVisible())
            continue;
        region += w->geometry();
    }
    return region;
}

QRegion ShapedWidget::appendRegion(QWidget *_widget, const QRegion &_maskRegion)
{
    QRegion region = _maskRegion;
    return (region += _widget->geometry());
}

QRegion ShapedWidget::removeRegion(QWidget *_widget, const QRegion &_maskRegion)
{
    QRegion region = _maskRegion;
    return (region -= _widget->geometry());
}

