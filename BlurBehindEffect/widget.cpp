#include "widget.h"
#include "wigglywidget.h"
#include "blurbehindeffect.h"

#include <QPainter>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    container = new QWidget(this);

    BlurBehindEffect* effect = new BlurBehindEffect(container);
    effect->setBlurMethod(BlurBehindEffect::BlurMethod::StackBlur);
    effect->setDownsampleFactor(3.0);
    effect->setBlurRadius(2);
    effect->setBlurOpacity(0.8);
    effect->setSourceOpacity(1.0);
    effect->setBackgroundBrush(QColor(0, 255, 0, 96));
    container->setGraphicsEffect(effect);

    overlay = new OverlayWidget(effect, this);

    QFormLayout* formLayout = new QFormLayout(container);
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
    layout->addWidget(overlay);
    layout->addWidget(container);

    ControlPanel* panel = new ControlPanel(this);
    panel->setWindowFlags(Qt::Tool);
    panel->setEffect(effect);
    panel->setWidget(overlay);
    panel->show();
}

Widget::~Widget()
{
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::lightGray);
}



OverlayWidget::OverlayWidget(BlurBehindEffect* effect, QWidget* parent)
    : QWidget(parent)
    , mEffect(effect)
{
    setAttribute(Qt::WA_TranslucentBackground, true);

    if (mEffect)
    {
        connect(mEffect, &BlurBehindEffect::repaintRequired, this, qOverload<>(&OverlayWidget::repaint));
        connect(mEffect, &QGraphicsEffect::enabledChanged, this, &OverlayWidget::setVisible);
    }
}

void OverlayWidget::paintEvent(QPaintEvent*)
{
    const QWidget* parentWdt = parentWidget();
    const QRect rc = parentWdt ? parentWdt->rect() : QRect{};
    const QPoint p = geometry().topLeft() - rc.topLeft();

    QPainter painter(this);
    if (mEffect)
    {
        painter.save();
        if (p.x() < 0)
            painter.translate(-p.x(), 0);
        if (p.y() < 0)
            painter.translate(0, -p.y());
        mEffect->render(&painter);
        painter.restore();
    }
}

void OverlayWidget::resizeEvent(QResizeEvent *event)
{
    const QRect r = rect();

    if (mEffect)
        mEffect->setRegion(parentWidget() ? geometry() & parentWidget()->rect() : r);
    QWidget::resizeEvent(event);
}



ControlPanel::ControlPanel(QWidget *parent)
{
    initUi();
}

void ControlPanel::initUi()
{
    blurMethodBox = new QComboBox(this);
    const QMetaEnum metaEnum = QMetaEnum::fromType<BlurBehindEffect::BlurMethod>();
    for (int i = 0; i < metaEnum.keyCount(); ++i)
        blurMethodBox->addItem(metaEnum.key(i), metaEnum.value(i));

    blurRadiusBox = new QSpinBox(this);
    blurRadiusBox->setRange(2, 12);

    blurOpacityBox = new QDoubleSpinBox(this);
    blurOpacityBox->setSingleStep(0.1);
    blurOpacityBox->setRange(0.0, 1.0);

    sourceOpacityBox = new QDoubleSpinBox(this);
    sourceOpacityBox->setSingleStep(0.1);
    sourceOpacityBox->setRange(0.0, 1.0);

    downsampleBox = new QDoubleSpinBox(this);
    downsampleBox->setSingleStep(0.5);
    downsampleBox->setRange(1.0, 10.0);

    brushBox = new QCheckBox(this);
    brushBox->setChecked(false);

    rColorBox = new QSpinBox(this);
    rColorBox->setRange(0, 255);
    rColorBox->setEnabled(brushBox->isChecked());
    connect(brushBox, &QCheckBox::toggled, rColorBox, &QSpinBox::setEnabled);

    gColorBox = new QSpinBox(this);
    gColorBox->setRange(0, 255);
    gColorBox->setEnabled(brushBox->isChecked());
    connect(brushBox, &QCheckBox::toggled, gColorBox, &QSpinBox::setEnabled);

    bColorBox = new QSpinBox(this);
    bColorBox->setRange(0, 255);
    bColorBox->setEnabled(brushBox->isChecked());
    connect(brushBox, &QCheckBox::toggled, bColorBox, &QSpinBox::setEnabled);

    aColorBox = new QSpinBox(this);
    aColorBox->setRange(0, 255);
    aColorBox->setEnabled(brushBox->isChecked());
    connect(brushBox, &QCheckBox::toggled, aColorBox, &QSpinBox::setEnabled);

    enabledBox = new QCheckBox(this);

    QFormLayout* formLayout = new QFormLayout(this);
    formLayout->addRow(tr("Blur Method:"), blurMethodBox);
    formLayout->addRow(tr("Blur Radius:"), blurRadiusBox);
    formLayout->addRow(tr("Blur Opacity:"), blurOpacityBox);
    formLayout->addRow(tr("Source Opacity:"), sourceOpacityBox);
    formLayout->addRow(tr("Downsample Factor:"), downsampleBox);
    formLayout->addRow(tr("Blur Background Brush:"), brushBox);
    formLayout->addRow(tr("Blur Color (red):"), rColorBox);
    formLayout->addRow(tr("Blur Color (green):"), gColorBox);
    formLayout->addRow(tr("Blur Color (blue):"), bColorBox);
    formLayout->addRow(tr("Blur Color (alpha):"), aColorBox);
    formLayout->addRow(tr("Enable effect:"), enabledBox);
}

void ControlPanel::updateValues()
{
    if (blurEffect)
    {
        blurMethodBox->setCurrentIndex(blurMethodBox->findData((int)blurEffect->blurMethod()));
        blurRadiusBox->setValue(blurEffect->blurRadius());
        blurOpacityBox->setValue(blurEffect->blurOpacity());
        sourceOpacityBox->setValue(blurEffect->sourceOpacity());
        downsampleBox->setValue(blurEffect->downsampleFactor());
        brushBox->setChecked(blurEffect->backgroundBrush().style() != Qt::NoBrush);

        const QColor c = blurEffect->backgroundBrush().color();
        rColorBox->setValue(c.red());
        gColorBox->setValue(c.green());
        bColorBox->setValue(c.blue());
        aColorBox->setValue(c.alpha());

        enabledBox->setChecked(blurEffect->isEnabled());
    }
}

void ControlPanel::setupValues()
{
    if (blurEffect)
    {
        blurEffect->setBlurMethod((BlurBehindEffect::BlurMethod)blurMethodBox->currentData().toInt());
        blurEffect->setBlurRadius(blurRadiusBox->value());
        blurEffect->setBlurOpacity(blurOpacityBox->value());
        blurEffect->setSourceOpacity(sourceOpacityBox->value());
        blurEffect->setDownsampleFactor(downsampleBox->value());
        if (!brushBox->isChecked())
        {
            blurEffect->setBackgroundBrush(Qt::NoBrush);
        }
        else
        {
            QColor c;
            c.setRed(rColorBox->value());
            c.setGreen(gColorBox->value());
            c.setBlue(bColorBox->value());
            c.setAlpha(aColorBox->value());
            blurEffect->setBackgroundBrush(c);
        }
        blurEffect->setEnabled(enabledBox->isChecked());
    }
}

void ControlPanel::setEffect(BlurBehindEffect *e)
{
    typedef void(QComboBox::* CBIndexChanged)(int);
    typedef void(QSpinBox::* SBIndexChanged)(int);
    typedef void(QDoubleSpinBox::* DSBIndexChanged)(double);

    blurEffect = e;
    updateValues();

    connect(blurMethodBox, static_cast<CBIndexChanged>(&QComboBox::currentIndexChanged), this, &ControlPanel::setupValues);
    connect(blurRadiusBox, static_cast<SBIndexChanged>(&QSpinBox::valueChanged), this, &ControlPanel::setupValues);
    connect(blurOpacityBox, static_cast<DSBIndexChanged>(&QDoubleSpinBox::valueChanged), this, &ControlPanel::setupValues);
    connect(downsampleBox, static_cast<DSBIndexChanged>(&QDoubleSpinBox::valueChanged), this, &ControlPanel::setupValues);
    connect(sourceOpacityBox, static_cast<DSBIndexChanged>(&QDoubleSpinBox::valueChanged), this, &ControlPanel::setupValues);

    connect(brushBox, &QCheckBox::toggled, this, &ControlPanel::setupValues);
    connect(gColorBox, static_cast<SBIndexChanged>(&QSpinBox::valueChanged), this, &ControlPanel::setupValues);
    connect(rColorBox, static_cast<SBIndexChanged>(&QSpinBox::valueChanged), this, &ControlPanel::setupValues);
    connect(bColorBox, static_cast<SBIndexChanged>(&QSpinBox::valueChanged), this, &ControlPanel::setupValues);
    connect(enabledBox, &QCheckBox::toggled, blurEffect, &QGraphicsEffect::setEnabled);
}

BlurBehindEffect *ControlPanel::effect() const
{
    return blurEffect;
}

void ControlPanel::setWidget(OverlayWidget *overlay)
{
    overlayWidget = overlay;
    updateValues();
}

OverlayWidget *ControlPanel::widget() const
{
    return overlayWidget;
}


