#include "brushpreview.h"
#include <QPainter>
#include <QColorDialog>
#include <QMouseEvent>

BrushPreview::BrushPreview(QWidget *parent)
    : QWidget(parent)
    , color_(Qt::transparent)
    , width_(0)
    , hasColorDialog_(true)
    , pressed_(false)
{
    setAttribute(Qt::WA_Hover);
    setColorDialogEnabled(true);
}

void BrushPreview::setColorDialogEnabled(bool _on)
{
    hasColorDialog_ = _on;
    setCursor(hasColorDialog_ ? Qt::PointingHandCursor : Qt::ArrowCursor);
}

bool BrushPreview::isColorDialogEnabled() const
{
    return hasColorDialog_;
}

void BrushPreview::updateColor(QColor _color)
{
    if (color_ == _color)
        return;

    color_ = _color;
    update();
}

void BrushPreview::updateThickness(int _value)
{
    if (width_ == _value)
        return;

    width_ = _value;
    update();
}

void BrushPreview::paintEvent(QPaintEvent*)
{
    QRect part({}, rect().size() / 2);
    QPainter painter(this);

    // draw chess-like background
    painter.fillRect(rect(), Qt::white);
    painter.fillRect(part, Qt::gray);
    part.moveTo(part.bottomRight());
    painter.fillRect(part, Qt::gray);

    // draw frame
    QColor borderColor = color_;
    borderColor.setAlpha(255);
    painter.setPen(QPen(borderColor, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect().adjusted(1, 1, -1, -1));

    // draw brush
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color_);
    painter.drawEllipse(rect().center() + QPoint(1, 1), width_ / 2, width_ / 2);
}

void BrushPreview::mousePressEvent(QMouseEvent *_event)
{
    if (!hasColorDialog_ || _event->button() != Qt::LeftButton)
        return QWidget::mousePressEvent(_event);

    pressed_ = true;
}

void BrushPreview::mouseReleaseEvent(QMouseEvent* _event)
{
    if (!pressed_)
        return QWidget::mouseReleaseEvent(_event);

    const QColor color = QColorDialog::getColor(color_, this, tr("Select Color"));
    if (!color.isValid())
        return QWidget::mouseReleaseEvent(_event);

    color_ = color;
    repaint();
    Q_EMIT colorChanged(color);
}
