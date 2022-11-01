#include "blurbehindeffect.h"
#include "glblurfunctions.h"
#include "blur.h"

#include <QPainter>
#include <QWidget>
#include <QThread>

class BlurBehindEffectPrivate
{
public:
    GLBlurFunctions glBlur_;
    qint64 cacheKey_;
    QImage sourceImage_;
    QImage blurredImage_;
    QRegion region_;
    BlurBehindEffect::BlurMethod blurringMethod_;
    Qt::CoordinateSystem coordSystem_;
    QBrush backgroundBrush_;
    double sourceOpacity_;
    double blurOpacity_;
    double downsamplingFactor_;
    int blurRadius_;
    int maxThreadCount_;
    bool sourceUpdated_;

    BlurBehindEffectPrivate()
        : cacheKey_(0)
        , blurringMethod_(BlurBehindEffect::BlurMethod::StackBlur)
        , coordSystem_(Qt::LogicalCoordinates)
        , sourceOpacity_(1.0)
        , blurOpacity_(1.0)
        , downsamplingFactor_(2.0)
        , blurRadius_(2)
        , maxThreadCount_(1)
        , sourceUpdated_(true)
    {
    }

    QImage blurImage(const QImage &_input)
    {
        switch(blurringMethod_)
        {
        case BlurBehindEffect::BlurMethod::BoxBlur:
            return boxBlurImage(_input, blurRadius_);
        case BlurBehindEffect::BlurMethod::StackBlur:
            return stackBlurImage(_input, blurRadius_, maxThreadCount_);
        case BlurBehindEffect::BlurMethod::GLBlur:
            return glBlur_.blurImage_DualKawase(_input, 2, std::max(blurRadius_ - 2, 1));
        }
        return _input;
    }

    QPixmap grabSource(QWidget* _widget) const
    {
        if (!_widget)
            return QPixmap{};

        qreal dpr(1.0);
        if (const auto *paintDevice = _widget)
            dpr = paintDevice->devicePixelRatioF();
        else
            qWarning("QtBlurBehindEffect::grabSource: Painter not active");

        const bool isGlBlur = blurringMethod_ == BlurBehindEffect::BlurMethod::GLBlur;
        QPixmap image(_widget->size());
        image.setDevicePixelRatio(dpr);
        image.fill(isGlBlur ? _widget->palette().color(_widget->backgroundRole()) : Qt::transparent);
        _widget->render(&image, {}, {}, QWidget::DrawChildren);

        return image;
    }

    void renderImage(QPainter *_painter, const QImage &_image, const QBrush& _brush)
    {
        if (region_.isEmpty() || _image.isNull())
            return;

        const QRect bounds = region_.boundingRect();
        const QSize s = bounds.size();
        QRect r(QPoint(0, 0), s);

        QTransform transform;
        if (coordSystem_ == Qt::LogicalCoordinates)
            transform = _painter->worldTransform();

        _painter->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
        if (coordSystem_ == Qt::LogicalCoordinates)
            _painter->translate(bounds.topLeft());

        if (!_brush.isOpaque() && !_brush.style() == Qt::NoBrush)
            _painter->fillRect(r, _brush);
        _painter->setOpacity(blurOpacity_);
        _painter->drawImage(r, _image);

        if (coordSystem_ == Qt::LogicalCoordinates)
            _painter->setWorldTransform(transform);
    }
};

BlurBehindEffect::BlurBehindEffect(QWidget *_parent)
    : QGraphicsEffect(_parent)
    , d(std::make_unique<BlurBehindEffectPrivate>())
{
}

BlurBehindEffect::~BlurBehindEffect() = default;

void BlurBehindEffect::setSourceOpacity(double _opacity)
{
    _opacity = std::clamp(_opacity, 0.0, 1.0);
    if (d->sourceOpacity_ == _opacity)
        return;

    d->sourceOpacity_ = std::clamp(_opacity, 0.0, 1.0);
    Q_EMIT sourceOpacityChanged(_opacity);
    Q_EMIT repaintRequired();
    update();
}

double BlurBehindEffect::sourceOpacity() const
{
    return d->sourceOpacity_;
}

void BlurBehindEffect::setBlurOpacity(double _opacity)
{
    _opacity = std::clamp(_opacity, 0.0, 1.0);
    if (d->blurOpacity_ == _opacity)
        return;

    d->blurOpacity_ = _opacity;
    Q_EMIT blurOpacityChanged(_opacity);
    Q_EMIT repaintRequired();
    update();
}

double BlurBehindEffect::blurOpacity() const
{
    return d->blurOpacity_;
}

void BlurBehindEffect::setBackgroundBrush(const QBrush& _brush)
{
    if (d->backgroundBrush_ == _brush)
        return;

    d->backgroundBrush_ = _brush;

    Q_EMIT backgroundBrushChanged(_brush);
    Q_EMIT repaintRequired();
    update();
}

QBrush BlurBehindEffect::backgroundBrush() const
{
    return d->backgroundBrush_;
}

void BlurBehindEffect::setBlurMethod(BlurBehindEffect::BlurMethod _method)
{
    if (d->blurringMethod_ == _method)
        return;

    d->blurringMethod_ = _method;
    Q_EMIT repaintRequired();
    update();
}

BlurBehindEffect::BlurMethod BlurBehindEffect::blurMethod() const
{
    return d->blurringMethod_;
}

void BlurBehindEffect::setRegion(const QRegion& _sourceRegion)
{
    d->region_ = _sourceRegion;
    updateBoundingRect();
}

const QRegion &BlurBehindEffect::region() const
{
    return d->region_;
}

void BlurBehindEffect::setBlurRadius(int _radius)
{
    if (d->blurRadius_ == _radius)
        return;

    d->blurRadius_ = _radius;
    Q_EMIT blurRadiusChanged(_radius);
    Q_EMIT repaintRequired();

    updateBoundingRect();
}

int BlurBehindEffect::blurRadius() const
{
    return d->blurRadius_;
}

void BlurBehindEffect::setDownsampleFactor(double _factor)
{
    _factor = std::max(_factor, 1.0);

    if (d->downsamplingFactor_ == _factor)
        return;

    d->downsamplingFactor_ = _factor;
    Q_EMIT downsampleFactorChanged(_factor);
    Q_EMIT repaintRequired();

    update();
}

double BlurBehindEffect::downsampleFactor() const
{
    return d->downsamplingFactor_;
}

void BlurBehindEffect::setMaxThreadCount(int _nthreads)
{
    d->maxThreadCount_ = std::clamp(_nthreads, 1, std::max(QThread::idealThreadCount(), 1));
}

int BlurBehindEffect::maxThreadCount() const
{
    return d->maxThreadCount_;
}

void BlurBehindEffect::setCoordinateSystem(Qt::CoordinateSystem _system)
{
    if (d->coordSystem_ == _system)
        return;

    d->coordSystem_ = _system;
    Q_EMIT repaintRequired();
    update();
}

Qt::CoordinateSystem BlurBehindEffect::coordinateSystem() const
{
    return d->coordSystem_;
}

void BlurBehindEffect::draw(QPainter* _painter)
{
    QWidget* w = qobject_cast<QWidget*>(parent());
    if (!w)
        return;

    const QRect bounds = d->region_.boundingRect();

    // grap widget source pixmap
    QPixmap pixmap = d->grabSource(w);
    // render source
    _painter->drawPixmap(0, 0, pixmap);

    if (d->sourceOpacity_ > 0.0 && d->sourceOpacity_ < 1.0)
    {
        if (!d->region_.isEmpty() && bounds != w->rect())
            _painter->setClipRegion(QRegion(w->rect()) -= d->region_);
        else if (!d->region_.isEmpty())
            _painter->setClipRegion(d->region_);

        const double opacity = _painter->opacity();
        _painter->setOpacity(d->sourceOpacity_);
        _painter->drawPixmap(0, 0, pixmap);
        _painter->setOpacity(opacity);
    }

    // get image blur region and downsample it
    if (!d->region_.isEmpty() && d->blurRadius_ > 1)
    {
        const double dpr = pixmap.devicePixelRatioF();
        const QSize s = (QSizeF(bounds.size()) * (dpr / d->downsamplingFactor_)).toSize();
        QImage pixmapPart = std::move(pixmap.copy(bounds).scaled(s, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).toImage());
        if (d->sourceImage_ != pixmapPart)
        {
            d->sourceImage_ = pixmapPart;
            d->cacheKey_ = pixmapPart.cacheKey();
            d->sourceUpdated_ = true;
        }
    }
}

void BlurBehindEffect::render(QPainter* _painter)
{
    if (blurRadius() <= 1 || d->sourceImage_.isNull())
        return;

    if (!d->sourceUpdated_)
        return d->renderImage(_painter, d->blurredImage_, d->backgroundBrush_);

    d->blurredImage_ = std::move(d->blurImage(d->sourceImage_));
    d->renderImage(_painter, d->blurredImage_, d->backgroundBrush_);
    d->sourceUpdated_ = false;
}

void BlurBehindEffect::render(QPainter* _painter, const QPainterPath& _clipPath)
{
    if (blurRadius() <= 1 || d->sourceImage_.isNull())
        return;

    if (_clipPath.isEmpty())
        return render(_painter);

    const QRect regionRect = d->region_.boundingRect();
    const QRectF targetBounds = _clipPath.boundingRect();

    if (!regionRect.contains(targetBounds.toRect()))
    {
        qWarning() << "target path is outside of source region";
        return;
    }

    if (d->sourceUpdated_)
    {
        d->blurredImage_ = std::move(d->blurImage(d->sourceImage_));
        d->sourceUpdated_ = false;
    }

    QImage image = d->blurredImage_.scaled(regionRect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    _painter->setOpacity(d->blurOpacity_);
    _painter->drawImage(QPointF{}, image, targetBounds);
}

