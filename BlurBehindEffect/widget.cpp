#include "widget.h"
#include "wigglywidget.h"
#include "blurbehindeffect.h"

#include <QPainter>

Widget::Widget(QWidget* _parent)
    : QWidget(_parent)
{
    container_ = new QWidget(this);

    BlurBehindEffect* effect = new BlurBehindEffect(container_);
    effect->setBlurMethod(BlurBehindEffect::BlurMethod::StackBlur);
    effect->setDownsampleFactor(3.0);
    effect->setBlurRadius(2);
    effect->setBlurOpacity(0.8);
    effect->setSourceOpacity(1.0);
    effect->setBackgroundBrush(QColor(0, 255, 0, 96));
    container_->setGraphicsEffect(effect);

    overlay_ = new OverlayWidget(effect, this);

    QFormLayout* formLayout = new QFormLayout(container_);
    formLayout->addRow(tr("Button"), new QPushButton(tr("Push Me!"), this));
    formLayout->addRow(tr("Label"), new QPushButton(tr("Text Label!"), this));
    formLayout->addRow(tr("Button"), new QRadioButton(tr("QRadioButton 1"), this));
    formLayout->addRow(tr("Button"), new QRadioButton(tr("QRadioButton 2"), this));
    formLayout->addRow(tr("Button"), new QRadioButton(tr("QRadioButton 3"), this));
    formLayout->addRow(tr("Button"), new QRadioButton(tr("QRadioButton 4"), this));
    formLayout->addRow(tr("Button"), new QCheckBox(tr("QCheckBox 1"), this));
    formLayout->addRow(tr("Button"), new QCheckBox(tr("QCheckBox 2"), this));
    formLayout->addRow(tr("Button"), new QCheckBox(tr("QCheckBox 3"), this));
    formLayout->addRow(tr("Button"), new QCheckBox(tr("QCheckBox 4"), this));
    formLayout->addRow(tr("Wiggly"), new WigglyWidget(this));

    QStackedLayout* layout = new QStackedLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setStackingMode(QStackedLayout::StackAll);
    layout->addWidget(overlay_);
    layout->addWidget(container_);

    ControlPanel* panel = new ControlPanel(this);
    panel->setWindowFlags(Qt::Tool);
    panel->setEffect(effect);
    panel->setWidget(overlay_);
    panel->show();
    connect(panel, &ControlPanel::closed, this, &Widget::close);
}

Widget::~Widget()
{
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::lightGray);
}



OverlayWidget::OverlayWidget(BlurBehindEffect* _effect, QWidget* _parent)
    : QWidget(_parent)
    , effect_(_effect)
{
    setAttribute(Qt::WA_TranslucentBackground, true);

    if (effect_)
    {
        connect(effect_, &BlurBehindEffect::repaintRequired, this, qOverload<>(&OverlayWidget::repaint));
        connect(effect_, &QGraphicsEffect::enabledChanged, this, &OverlayWidget::setVisible);
    }
}

void OverlayWidget::paintEvent(QPaintEvent*)
{
    const QWidget* parentWdt = parentWidget();
    const QRect rc = parentWdt ? parentWdt->rect() : QRect{};
    const QPoint p = geometry().topLeft() - rc.topLeft();

    QPainter painter(this);
    if (effect_)
    {
        painter.save();
        if (p.x() < 0)
            painter.translate(-p.x(), 0);
        if (p.y() < 0)
            painter.translate(0, -p.y());
        effect_->render(&painter);
        painter.restore();
    }
}

void OverlayWidget::resizeEvent(QResizeEvent* _event)
{
    const QRect r = rect();

    if (effect_)
        effect_->setRegion(parentWidget() ? geometry() & parentWidget()->rect() : r);
    QWidget::resizeEvent(_event);
}



ControlPanel::ControlPanel(QWidget* _parent)
{
    initUi();
}

void ControlPanel::initUi()
{
    blurMethodBox_ = new QComboBox(this);
    const QMetaEnum metaEnum = QMetaEnum::fromType<BlurBehindEffect::BlurMethod>();
    for (int i = 0; i < metaEnum.keyCount(); ++i)
        blurMethodBox_->addItem(metaEnum.key(i), metaEnum.value(i));

    blurRadiusBox_ = new QSpinBox(this);
    blurRadiusBox_->setRange(2, 12);

    blurOpacityBox_ = new QDoubleSpinBox(this);
    blurOpacityBox_->setSingleStep(0.1);
    blurOpacityBox_->setRange(0.0, 1.0);

    sourceOpacityBox_ = new QDoubleSpinBox(this);
    sourceOpacityBox_->setSingleStep(0.1);
    sourceOpacityBox_->setRange(0.0, 1.0);

    downsampleBox_ = new QDoubleSpinBox(this);
    downsampleBox_->setSingleStep(0.5);
    downsampleBox_->setRange(1.0, 10.0);

    brushBox_ = new QCheckBox(this);
    brushBox_->setChecked(false);

    rColorBox_ = new QSpinBox(this);
    rColorBox_->setRange(0, 255);
    rColorBox_->setEnabled(brushBox_->isChecked());
    connect(brushBox_, &QCheckBox::toggled, rColorBox_, &QSpinBox::setEnabled);

    gColorBox_ = new QSpinBox(this);
    gColorBox_->setRange(0, 255);
    gColorBox_->setEnabled(brushBox_->isChecked());
    connect(brushBox_, &QCheckBox::toggled, gColorBox_, &QSpinBox::setEnabled);

    bColorBox_ = new QSpinBox(this);
    bColorBox_->setRange(0, 255);
    bColorBox_->setEnabled(brushBox_->isChecked());
    connect(brushBox_, &QCheckBox::toggled, bColorBox_, &QSpinBox::setEnabled);

    aColorBox_ = new QSpinBox(this);
    aColorBox_->setRange(0, 255);
    aColorBox_->setEnabled(brushBox_->isChecked());
    connect(brushBox_, &QCheckBox::toggled, aColorBox_, &QSpinBox::setEnabled);

    enabledBox_ = new QCheckBox(this);

    QFormLayout* formLayout = new QFormLayout(this);
    formLayout->addRow(tr("Blur Method:"), blurMethodBox_);
    formLayout->addRow(tr("Blur Radius:"), blurRadiusBox_);
    formLayout->addRow(tr("Blur Opacity:"), blurOpacityBox_);
    formLayout->addRow(tr("Source Opacity:"), sourceOpacityBox_);
    formLayout->addRow(tr("Downsample Factor:"), downsampleBox_);
    formLayout->addRow(tr("Blur Background Brush:"), brushBox_);
    formLayout->addRow(tr("Blur Color (red):"), rColorBox_);
    formLayout->addRow(tr("Blur Color (green):"), gColorBox_);
    formLayout->addRow(tr("Blur Color (blue):"), bColorBox_);
    formLayout->addRow(tr("Blur Color (alpha):"), aColorBox_);
    formLayout->addRow(tr("Enable effect:"), enabledBox_);
}

void ControlPanel::updateValues()
{
    if (blurEffect_)
    {
        blurMethodBox_->setCurrentIndex(blurMethodBox_->findData((int)blurEffect_->blurMethod()));
        blurRadiusBox_->setValue(blurEffect_->blurRadius());
        blurOpacityBox_->setValue(blurEffect_->blurOpacity());
        sourceOpacityBox_->setValue(blurEffect_->sourceOpacity());
        downsampleBox_->setValue(blurEffect_->downsampleFactor());
        brushBox_->setChecked(blurEffect_->backgroundBrush().style() != Qt::NoBrush);

        const QColor c = blurEffect_->backgroundBrush().color();
        rColorBox_->setValue(c.red());
        gColorBox_->setValue(c.green());
        bColorBox_->setValue(c.blue());
        aColorBox_->setValue(c.alpha());

        enabledBox_->setChecked(blurEffect_->isEnabled());
    }
}

void ControlPanel::setupValues()
{
    if (blurEffect_)
    {
        blurEffect_->setBlurMethod((BlurBehindEffect::BlurMethod)blurMethodBox_->currentData().toInt());
        blurEffect_->setBlurRadius(blurRadiusBox_->value());
        blurEffect_->setBlurOpacity(blurOpacityBox_->value());
        blurEffect_->setSourceOpacity(sourceOpacityBox_->value());
        blurEffect_->setDownsampleFactor(downsampleBox_->value());
        if (!brushBox_->isChecked())
        {
            blurEffect_->setBackgroundBrush(Qt::NoBrush);
        }
        else
        {
            QColor c;
            c.setRed(rColorBox_->value());
            c.setGreen(gColorBox_->value());
            c.setBlue(bColorBox_->value());
            c.setAlpha(aColorBox_->value());
            blurEffect_->setBackgroundBrush(c);
        }
        blurEffect_->setEnabled(enabledBox_->isChecked());
    }
}

void ControlPanel::setEffect(BlurBehindEffect* _effect)
{
    typedef void(QComboBox::* CBIndexChanged)(int);
    typedef void(QSpinBox::* SBIndexChanged)(int);
    typedef void(QDoubleSpinBox::* DSBIndexChanged)(double);

    blurEffect_ = _effect;
    updateValues();

    connect(blurMethodBox_, static_cast<CBIndexChanged>(&QComboBox::currentIndexChanged), this, &ControlPanel::setupValues);
    connect(blurRadiusBox_, static_cast<SBIndexChanged>(&QSpinBox::valueChanged), this, &ControlPanel::setupValues);
    connect(blurOpacityBox_, static_cast<DSBIndexChanged>(&QDoubleSpinBox::valueChanged), this, &ControlPanel::setupValues);
    connect(downsampleBox_, static_cast<DSBIndexChanged>(&QDoubleSpinBox::valueChanged), this, &ControlPanel::setupValues);
    connect(sourceOpacityBox_, static_cast<DSBIndexChanged>(&QDoubleSpinBox::valueChanged), this, &ControlPanel::setupValues);

    connect(brushBox_, &QCheckBox::toggled, this, &ControlPanel::setupValues);
    connect(gColorBox_, static_cast<SBIndexChanged>(&QSpinBox::valueChanged), this, &ControlPanel::setupValues);
    connect(rColorBox_, static_cast<SBIndexChanged>(&QSpinBox::valueChanged), this, &ControlPanel::setupValues);
    connect(bColorBox_, static_cast<SBIndexChanged>(&QSpinBox::valueChanged), this, &ControlPanel::setupValues);
    connect(enabledBox_, &QCheckBox::toggled, blurEffect_, &QGraphicsEffect::setEnabled);
}

BlurBehindEffect *ControlPanel::effect() const
{
    return blurEffect_;
}

void ControlPanel::setWidget(OverlayWidget* _overlay)
{
    overlayWidget_ = _overlay;
    updateValues();
}

OverlayWidget *ControlPanel::widget() const
{
    return overlayWidget_;
}

void ControlPanel::closeEvent(QCloseEvent *_event)
{
    QWidget::closeEvent(_event);
    Q_EMIT closed();
}


