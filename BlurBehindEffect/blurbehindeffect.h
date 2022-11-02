#pragma once
#include <memory>
#include <QGraphicsEffect>

class BlurBehindEffect :
        public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(BlurMethod blurMethod READ blurMethod WRITE setBlurMethod)
    Q_PROPERTY(int blurRadius READ blurRadius WRITE setBlurRadius NOTIFY blurRadiusChanged)
    Q_PROPERTY(double blurOpacity READ blurOpacity WRITE setBlurOpacity NOTIFY blurOpacityChanged)
    Q_PROPERTY(double sourceOpacity READ sourceOpacity WRITE setSourceOpacity NOTIFY sourceOpacityChanged)
    Q_PROPERTY(double downsampleFactor READ downsampleFactor WRITE setDownsampleFactor NOTIFY downsampleFactorChanged)
    Q_PROPERTY(QBrush backgroundBrush READ backgroundBrush WRITE setBackgroundBrush NOTIFY backgroundBrushChanged)

public:
    enum class BlurMethod
    {
        BoxBlur,
        StackBlur,
        GLBlur
    };
    Q_ENUM(BlurMethod)

    BlurBehindEffect(QWidget* _parent = nullptr);
    ~BlurBehindEffect();

    void render(QPainter* _painter);
    void render(QPainter* _painter, const QPainterPath& _clipPath);

    void setSourceOpacity(double _opacity);
    double sourceOpacity() const;

    void setBlurOpacity(double _opacity);
    double blurOpacity() const;

    void setBackgroundBrush(const QBrush& _brush);
    QBrush backgroundBrush() const;

    void setBlurMethod(BlurMethod _method);
    BlurMethod blurMethod() const;

    void setRegion(const QRegion& _sourceRegion);
    const QRegion& region() const;

    void setBlurRadius(int _radius);
    int blurRadius() const;

    void setDownsampleFactor(double _factor);
    double downsampleFactor() const;

    void setMaxThreadCount(int _nthreads);
    int maxThreadCount() const;

    void setCoordinateSystem(Qt::CoordinateSystem _system);
    Qt::CoordinateSystem coordinateSystem() const;

protected:
    void draw(QPainter *_painter) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void blurRadiusChanged(int);
    void blurOpacityChanged(double);
    void sourceOpacityChanged(double);
    void downsampleFactorChanged(double);
    void backgroundBrushChanged(const QBrush&);
    void repaintRequired();

private:
    std::unique_ptr<class BlurBehindEffectPrivate> d;
};

