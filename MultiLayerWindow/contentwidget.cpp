#include "contentwidget.h"
#include "coloredit.h"
#include "brushpreview.h"
#include <QtWidgets>
#include <QPainter>

namespace
{
    typedef void (QSpinBox::* sbValueChanged)(int);
    constexpr std::pair<int, int> thicknessRange { 1, 30 };
    constexpr int kDefaultPenWidth = 2;
    constexpr int kDefaultBrushWidth = 16;
    constexpr QPoint kInvalidPoint(-1, -1);
    constexpr QSize kBrushPreviewSize{ thicknessRange.second + 6, thicknessRange.second + 6 };

    QDialog* createPencilDialog(QDialog* _dialog, ContentWidget* _widget)
    {
        BrushPreview* preview = new BrushPreview(_dialog);
        preview->updateColor(_widget->pencilColor());
        preview->updateThickness(_widget->pencilWidth());
        preview->setFixedSize(kBrushPreviewSize);
        QObject::connect(preview, &BrushPreview::colorChanged, _widget, &ContentWidget::setPencilColor);

        ColorEdit* colorEdit = new ColorEdit(_dialog);
        colorEdit->setChannels(ColorEdit::Red|ColorEdit::Green|ColorEdit::Blue);
        colorEdit->setColor(_widget->pencilColor());
        QObject::connect(preview, &BrushPreview::colorChanged, colorEdit, &ColorEdit::setColor);
        QObject::connect(colorEdit, &ColorEdit::colorChanged, preview, &BrushPreview::updateColor);
        QObject::connect(colorEdit, &ColorEdit::colorChanged, _widget, &ContentWidget::setPencilColor);

        QSpinBox* thicknessEdit = new QSpinBox(_dialog);
        thicknessEdit->setRange(thicknessRange.first, thicknessRange.second);
        thicknessEdit->setValue(_widget->pencilWidth());
        QObject::connect(thicknessEdit, static_cast<sbValueChanged>(&QSpinBox::valueChanged), _widget, &ContentWidget::setPencilWidth);
        QObject::connect(thicknessEdit, static_cast<sbValueChanged>(&QSpinBox::valueChanged), preview, &BrushPreview::updateThickness);

        QHBoxLayout* layout = new QHBoxLayout;
        layout->addWidget(preview, 0, Qt::AlignCenter);
        layout->addWidget(colorEdit);

        QFormLayout* formLayout = new QFormLayout(_dialog);
        formLayout->addRow(layout);
        formLayout->addRow(QObject::tr("Thickness:"), thicknessEdit);

        return _dialog;
    }

    QDialog* createBrushDialog(QDialog* _dialog, ContentWidget* _widget)
    {
        BrushPreview* preview = new BrushPreview(_dialog);
        preview->updateColor(_widget->brushColor());
        preview->updateThickness(_widget->brushWidth());
        preview->setFixedSize(kBrushPreviewSize);
        QObject::connect(preview, &BrushPreview::colorChanged, _widget, &ContentWidget::setBrushColor);

        ColorEdit* colorEdit = new ColorEdit(_dialog);
        colorEdit->setChannels(ColorEdit::Red|ColorEdit::Green|ColorEdit::Blue|ColorEdit::Alpha);
        colorEdit->setColor(_widget->brushColor());
        QObject::connect(preview, &BrushPreview::colorChanged, colorEdit, &ColorEdit::setColor);
        QObject::connect(colorEdit, &ColorEdit::colorChanged, preview, &BrushPreview::updateColor);
        QObject::connect(colorEdit, &ColorEdit::colorChanged, _widget, &ContentWidget::setBrushColor);

        QSpinBox* thicknessEdit = new QSpinBox(_dialog);
        thicknessEdit->setRange(thicknessRange.first + 1, thicknessRange.second);
        thicknessEdit->setValue(_widget->brushWidth());
        QObject::connect(thicknessEdit, static_cast<sbValueChanged>(&QSpinBox::valueChanged), _widget, &ContentWidget::setBrushWidth);
        QObject::connect(thicknessEdit, static_cast<sbValueChanged>(&QSpinBox::valueChanged), preview, &BrushPreview::updateThickness);

        QHBoxLayout* layout = new QHBoxLayout;
        layout->addWidget(preview, 0, Qt::AlignCenter);
        layout->addWidget(colorEdit);

        QFormLayout* formLayout = new QFormLayout(_dialog);
        formLayout->addRow(layout);
        formLayout->addRow(QObject::tr("Thickness:"), thicknessEdit);

        return _dialog;
    }

    QDialog* createEraserDialog(QDialog* _dialog, ContentWidget* _widget)
    {
        BrushPreview* preview = new BrushPreview(_dialog);
        preview->setColorDialogEnabled(false);
        preview->updateColor(Qt::black);
        preview->updateThickness(_widget->eraserWidth());
        preview->setFixedSize(thicknessRange.second + 4, thicknessRange.second + 4);

        QSpinBox* thicknessEdit = new QSpinBox(_dialog);
        thicknessEdit->setRange(thicknessRange.first, thicknessRange.second);
        thicknessEdit->setValue(_widget->eraserWidth());
        QObject::connect(thicknessEdit, static_cast<sbValueChanged>(&QSpinBox::valueChanged), _widget, &ContentWidget::setEraserWidth);
        QObject::connect(thicknessEdit, static_cast<sbValueChanged>(&QSpinBox::valueChanged), preview, &BrushPreview::updateThickness);

        QHBoxLayout* layout = new QHBoxLayout(_dialog);
        layout->addWidget(preview, 0, Qt::AlignCenter);
        layout->addWidget(thicknessEdit);

        return _dialog;
    }
}

ContentWidget::ContentWidget(QWidget *_parent)
    : QWidget(_parent)
    , lastPoint_(kInvalidPoint)
    , penColor_(Qt::green)
    , brushColor_(Qt::green)
    , clearColor_(Qt::white)
    , tool_(Pencil)
    , penWidth_(kDefaultPenWidth)
    , brushWidth_(kDefaultBrushWidth)
    , eraserWidth_(kDefaultPenWidth)
{
    brushColor_.setAlpha(1);
    currentClearColor_ = clearColor_;
    setAttribute(Qt::WA_StaticContents);
    updateCursor();
}

void ContentWidget::setCurrentTool(PaintTool _tool)
{
    if (tool_ == _tool)
        return;

    tool_ = _tool;
    updateCursor();
}

PaintTool ContentWidget::currentTool() const
{
    return tool_;
}

void ContentWidget::setPencilColor(QColor _color)
{
    if (penColor_ == _color)
        return;
    penColor_ = _color;
    Q_EMIT pencilColorChanged(penColor_);
}

QColor ContentWidget::pencilColor() const
{
    return penColor_;
}

void ContentWidget::setBrushColor(QColor _color)
{
    if (brushColor_ == _color)
        return;
    brushColor_ = _color;
    Q_EMIT brushColorChanged(brushColor_);
}

QColor ContentWidget::brushColor() const
{
    return brushColor_;
}

void ContentWidget::setClearColor(QColor _color)
{
    if (clearColor_ == _color)
        return;

    clearColor_ = _color;
    Q_EMIT clearColorChanged(clearColor_);
}

QColor ContentWidget::clearColor() const
{
    return clearColor_;
}

void ContentWidget::setPencilWidth(int _w)
{
    if (penWidth_ == _w)
        return;

    penWidth_ = _w;
    updateCursor();
}

int ContentWidget::pencilWidth() const
{
    return penWidth_;
}

void ContentWidget::setBrushWidth(int _w)
{
    if (brushWidth_ == _w)
        return;

    brushWidth_ = _w;
    updateCursor();
}

int ContentWidget::brushWidth() const
{
    return brushWidth_;
}

void ContentWidget::setEraserWidth(int _w)
{
    if (eraserWidth_ == _w)
        return;

    eraserWidth_ = _w;
    updateCursor();
}

int ContentWidget::eraserWidth() const
{
    return eraserWidth_;
}

void ContentWidget::setImage(const QImage &_image)
{
    image_ = _image;
}

const QImage &ContentWidget::image() const
{
    return image_;
}

void ContentWidget::clear()
{
    currentClearColor_ = clearColor_;
    image_.fill(currentClearColor_);
    update();
}

void ContentWidget::paintEvent(QPaintEvent *_event)
{
    QPainter painter(this);
    const QRect dirtyRect = _event->rect();
    painter.drawImage(dirtyRect, image_, dirtyRect);
}

void ContentWidget::resizeEvent(QResizeEvent *_event)
{
    if (width() > image_.width() || height() > image_.height())
    {
        const int newWidth = qMax(width() + 128, image_.width());
        const int newHeight = qMax(height() + 128, image_.height());
        resizeImage(&image_, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(_event);
}

void ContentWidget::mousePressEvent(QMouseEvent *_event)
{
    if (_event->button() == Qt::LeftButton && _event->modifiers() == Qt::NoModifier)
        lastPoint_ = _event->pos();
}

void ContentWidget::mouseMoveEvent(QMouseEvent *_event)
{
    if ((_event->buttons() & Qt::LeftButton) && lastPoint_ != kInvalidPoint)
        drawLineTo(_event->pos());
}

void ContentWidget::mouseReleaseEvent(QMouseEvent *_event)
{
    if (_event->button() == Qt::LeftButton && lastPoint_ != kInvalidPoint)
    {
        drawLineTo(_event->pos());
        lastPoint_ = kInvalidPoint;
    }
}

void ContentWidget::drawLineTo(const QPoint &_endPoint)
{
    switch(tool_)
    {
    case Eraser:
        eraserLineTo(_endPoint);
        break;
    case Pencil:
        pencilLineTo(_endPoint);
        break;
    case Brush:
        brushLineTo(_endPoint);
        break;
    default:
        break;
    }
    lastPoint_ = _endPoint;
}

void ContentWidget::eraserLineTo(const QPoint &_endPoint)
{
    const int rad = (eraserWidth_ / 2) + 2;

    QPainter painter(&image_);
    painter.setPen(QPen(currentClearColor_, eraserWidth_, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(lastPoint_, _endPoint);

    update(QRect(lastPoint_, _endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
}

void ContentWidget::pencilLineTo(const QPoint &_endPoint)
{
    const int rad = (penWidth_ / 2) + 2;

    QPainter painter(&image_);
    painter.setPen(QPen(penColor_, penWidth_, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(lastPoint_, _endPoint);

    update(QRect(lastPoint_, _endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
}

void ContentWidget::brushLineTo(const QPoint &_endPoint)
{
    const int numSteps = 2 + (_endPoint - lastPoint_).manhattanLength() / 2;
    const int rad = brushWidth_ / 2;

    QPainter painter(&image_);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brushColor_);

    const int thickness = brushWidth_ / 2;
    for (int i = 0; i < numSteps; ++i)
    {
        int x = lastPoint_.x() + i * (_endPoint.x() - lastPoint_.x()) / (numSteps - 1);
        int y = lastPoint_.y() + i * (_endPoint.y() - lastPoint_.y()) / (numSteps - 1);

        painter.drawEllipse(x - thickness, y - thickness, brushWidth_, brushWidth_);
    }
    update(QRect(lastPoint_, _endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
}

void ContentWidget::updateCursor()
{
    QSize size;
    switch(tool_)
    {
    case Pencil:
        size.rwidth() = penWidth_;
        size.rheight() = penWidth_;
        break;
    case Brush:
        size.rwidth() = brushWidth_;
        size.rheight() = brushWidth_;
        break;
    case Eraser:
        size.rwidth() = eraserWidth_;
        size.rheight() = eraserWidth_;
        break;
    default:
        setCursor(Qt::CrossCursor);
        return;
    }

    if (size.width() < 2 || size.height() < 2)
    {
        setCursor(Qt::CrossCursor);
        return;
    }

    const QRect r(0, 0, size.width() - 1, size.height() - 1);

    QBitmap bitmap(size);
    bitmap.fill(Qt::color0);
    QPainter painter(&bitmap);
    painter.setPen(Qt::color1);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(r);

    setCursor(QCursor(bitmap, bitmap));
}

void ContentWidget::resizeImage(QImage *_image, const QSize &_newSize)
{
    if (_image->size() == _newSize)
        return;

    QImage newImage(_newSize, QImage::Format_ARGB32);
    newImage.fill(currentClearColor_);
    QPainter painter(&newImage);
    painter.drawImage(QPoint{}, *_image);
    *_image = newImage;
}

QDialog *ContentWidget::createEditDialog(PaintTool _tool, QWidget *_parent)
{
    switch(_tool)
    {
    case PaintTool::Pencil:
        return createPencilDialog(new QDialog(_parent, Qt::Popup), this);
    case PaintTool::Brush:
        return createBrushDialog(new QDialog(_parent, Qt::Popup), this);
    case PaintTool::Eraser:
        return createEraserDialog(new QDialog(_parent, Qt::Popup), this);
    default:
        break;
    }
    return Q_NULLPTR;
}


