#include "overlaypanel.h"
#include "blurbehindeffect.h"
#include <QPainter>
#include <QBitmap>

OverlayPanel::OverlayPanel(BlurBehindEffect *_effect, QWidget *_parent) : QWidget(_parent), effect_(_effect)
{
    if (effect_)
    {
        connect(effect_, &BlurBehindEffect::repaintRequired, this, qOverload<>(&OverlayPanel::repaint));
        connect(effect_, &QGraphicsEffect::enabledChanged, this, &OverlayPanel::setVisible);
    }

    setAttribute(Qt::WA_TranslucentBackground, true);
}

void OverlayPanel::resizeEvent(QResizeEvent *_event)
{
    QWidget::resizeEvent(_event);
    cachedPath_ = clipPath();
}

void OverlayPanel::moveEvent(QMoveEvent *_event)
{
    QWidget::moveEvent(_event);
}

void OverlayPanel::paintEvent(QPaintEvent* _event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
    painter.setClipPath(cachedPath_);

    QPainterPath path = cachedPath_;
    path.translate(geometry().topLeft());
    if (effect_)
        effect_->render(&painter, path);
    else
        QWidget::paintEvent(_event);
}

QPainterPath OverlayPanel::clipPath() const
{
    QPainterPath path;
    path.addRect(rect().adjusted(0, 0, -1, -1));
    return path;
}

