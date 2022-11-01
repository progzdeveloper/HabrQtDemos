#include "blurbehindeffect.h"
#include "glblurfunctions.h"
#include "blur.h"

#include <QPainter>
#include <QWidget>
#include <QThread>

class BlurBehindEffectPrivate
{
public:
    GLBlurFunctions glBlur;
    qint64 cacheKey;
    QImage sourceImage;
    QImage blurredImage;
    QRegion region;
    BlurBehindEffect::BlurMethod blurringMethod;
    Qt::CoordinateSystem coordSystem;
    QBrush backgroundBrush;
    double sourceOpacity;
    double blurOpacity;
    double downsamplingFactor;
    int blurRadius;
    int maxThreadCount;
    bool sourceUpdated;

    BlurBehindEffectPrivate()
        : cacheKey(0)
        , blurringMethod(BlurBehindEffect::BlurMethod::StackBlur)
        , coordSystem(Qt::LogicalCoordinates)
        , sourceOpacity(1.0)
        , blurOpacity(1.0)
        , downsamplingFactor(2.0)
        , blurRadius(2)
        , maxThreadCount(1)
        , sourceUpdated(true)
    {
    }

    QImage blurImage(const QImage &input)
    {
        switch(blurringMethod)
        {
        case BlurBehindEffect::BlurMethod::BoxBlur:
            return boxBlurImage(input, blurRadius);
        case BlurBehindEffect::BlurMethod::StackBlur:
            return stackBlurImage(input, blurRadius, maxThreadCount);
        case BlurBehindEffect::BlurMethod::GLBlur:
            return glBlur.blurImage_DualKawase(input, 2, std::max(blurRadius - 2, 1));
        }
        return input;
    }

    QPixmap grabSource(QWidget* widget) const
    {
        if (!widget)
            return QPixmap{};

        qreal dpr(1.0);
        if (const auto *paintDevice = widget)
            dpr = paintDevice->devicePixelRatioF();
        else
            qWarning("QtBlurBehindEffect::grabSource: Painter not active");

        const bool isGlBlur = blurringMethod == BlurBehindEffect::BlurMethod::GLBlur;
        QPixmap image(widget->size());
        image.setDevicePixelRatio(dpr);
        image.fill(isGlBlur ? widget->palette().color(widget->backgroundRole()) : Qt::transparent);
        widget->render(&image, {}, {}, QWidget::DrawChildren);

        return image;
    }

    void renderImage(QPainter *_painter, const QImage &_image, const QBrush& _brush)
    {
        if (region.isEmpty() || _image.isNull())
            return;

        const QRect bounds = region.boundingRect();
        const QSize s = bounds.size();
        QRect r(QPoint(0, 0), s);

        QTransform transform;
        if (coordSystem == Qt::LogicalCoordinates)
            transform = _painter->worldTransform();

        _painter->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
        if (coordSystem == Qt::LogicalCoordinates)
            _painter->translate(bounds.topLeft());

        if (!_brush.isOpaque() && !_brush.style() == Qt::NoBrush)
            _painter->fillRect(r, _brush);
        _painter->setOpacity(blurOpacity);
        _painter->drawImage(r, _image);

        if (coordSystem == Qt::LogicalCoordinates)
            _painter->setWorldTransform(transform);
    }
};

BlurBehindEffect::BlurBehindEffect(QWidget *parent)
    : QGraphicsEffect(parent)
    , d(std::make_unique<BlurBehindEffectPrivate>())
{
}

BlurBehindEffect::~BlurBehindEffect() = default;

void BlurBehindEffect::setSourceOpacity(double opacity)
{
    opacity = std::clamp(opacity, 0.0, 1.0);
    if (d->sourceOpacity == opacity)
        return;

    d->sourceOpacity = std::clamp(opacity, 0.0, 1.0);
    Q_EMIT sourceOpacityChanged(opacity);
    Q_EMIT repaintRequired();
    update();
}

double BlurBehindEffect::sourceOpacity() const
{
    return d->sourceOpacity;
}

void BlurBehindEffect::setBlurOpacity(double opacity)
{
    opacity = std::clamp(opacity, 0.0, 1.0);
    if (d->blurOpacity == opacity)
        return;

    d->blurOpacity = opacity;
    Q_EMIT blurOpacityChanged(opacity);
    Q_EMIT repaintRequired();
    update();
}

double BlurBehindEffect::blurOpacity() const
{
    return d->blurOpacity;
}

void BlurBehindEffect::setBackgroundBrush(const QBrush &brush)
{
    if (d->backgroundBrush == brush)
        return;

    d->backgroundBrush = brush;

    Q_EMIT backgroundBrushChanged(brush);
    Q_EMIT repaintRequired();
    update();
}

QBrush BlurBehindEffect::backgroundBrush() const
{
    return d->backgroundBrush;
}

void BlurBehindEffect::setBlurMethod(BlurBehindEffect::BlurMethod m)
{
    if (d->blurringMethod == m)
        return;

    d->blurringMethod = m;
    Q_EMIT repaintRequired();
    update();
}

BlurBehindEffect::BlurMethod BlurBehindEffect::blurMethod() const
{
    return d->blurringMethod;
}

void BlurBehindEffect::setRegion(const QRegion &r)
{
    d->region = r;
    updateBoundingRect();
}

const QRegion &BlurBehindEffect::region() const
{
    return d->region;
}

void BlurBehindEffect::setBlurRadius(int radius)
{
    if (d->blurRadius == radius)
        return;

    d->blurRadius = radius;
    Q_EMIT blurRadiusChanged(radius);
    Q_EMIT repaintRequired();

    updateBoundingRect();
}

int BlurBehindEffect::blurRadius() const
{
    return d->blurRadius;
}

void BlurBehindEffect::setDownsampleFactor(double factor)
{
    factor = std::max(factor, 1.0);

    if (d->downsamplingFactor == factor)
        return;

    d->downsamplingFactor = factor;
    Q_EMIT downsampleFactorChanged(factor);
    Q_EMIT repaintRequired();

    update();
}

double BlurBehindEffect::downsampleFactor() const
{
    return d->downsamplingFactor;
}

void BlurBehindEffect::setMaxThreadCount(int nthreads)
{
    d->maxThreadCount = std::clamp(nthreads, 1, std::max(QThread::idealThreadCount(), 1));
}

int BlurBehindEffect::maxThreadCount() const
{
    return d->maxThreadCount;
}

void BlurBehindEffect::setCoordinateSystem(Qt::CoordinateSystem system)
{
    if (d->coordSystem == system)
        return;

    d->coordSystem = system;
    Q_EMIT repaintRequired();
    update();
}

Qt::CoordinateSystem BlurBehindEffect::coordinateSystem() const
{
    return d->coordSystem;
}

void BlurBehindEffect::draw(QPainter *painter)
{
    QWidget* w = qobject_cast<QWidget*>(parent());
    if (!w)
        return;

    const QRect bounds = d->region.boundingRect();

    // grap widget source pixmap
    QPixmap pixmap = d->grabSource(w);
    // render source
    painter->drawPixmap(0, 0, pixmap);

    if (d->sourceOpacity > 0.0 && d->sourceOpacity < 1.0)
    {
        if (!d->region.isEmpty() && bounds != w->rect())
            painter->setClipRegion(QRegion(w->rect()) -= d->region);
        else if (!d->region.isEmpty())
            painter->setClipRegion(d->region);

        const double opacity = painter->opacity();
        painter->setOpacity(d->sourceOpacity);
        painter->drawPixmap(0, 0, pixmap);
        painter->setOpacity(opacity);
    }

    // get image blur region and downsample it
    if (!d->region.isEmpty() && d->blurRadius > 1)
    {
        const double dpr = pixmap.devicePixelRatioF();
        const QSize s = (QSizeF(bounds.size()) * (dpr / d->downsamplingFactor)).toSize();
        QImage pixmapPart = std::move(pixmap.copy(bounds).scaled(s, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).toImage());
        if (d->sourceImage != pixmapPart)
        {
            d->sourceImage = pixmapPart;
            d->cacheKey = pixmapPart.cacheKey();
            d->sourceUpdated = true;
        }
    }
}

void BlurBehindEffect::render(QPainter* _painter)
{
    if (blurRadius() <= 1 || d->sourceImage.isNull())
        return;

    if (!d->sourceUpdated)
        return d->renderImage(_painter, d->blurredImage, d->backgroundBrush);

    d->blurredImage = std::move(d->blurImage(d->sourceImage));
    d->renderImage(_painter, d->blurredImage, d->backgroundBrush);
    d->sourceUpdated = false;
}

void BlurBehindEffect::render(QPainter* _painter, const QPainterPath& _clipPath)
{
    if (blurRadius() <= 1 || d->sourceImage.isNull())
        return;

    if (_clipPath.isEmpty())
        return render(_painter);

    const QRect regionRect = d->region.boundingRect();
    const QRectF targetRect = _clipPath.boundingRect();

    if (!regionRect.intersects(targetRect.toRect()))
    {
        qWarning() << "target painter path is outside of source region";
        return;
    }

    if (d->sourceUpdated)
    {
        d->blurredImage = std::move(d->blurImage(d->sourceImage));
        d->sourceUpdated = false;
    }

    QImage image = d->blurredImage.scaled(regionRect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    _painter->setOpacity(d->blurOpacity);
    _painter->drawImage(QPointF{}, image, targetRect);
}

