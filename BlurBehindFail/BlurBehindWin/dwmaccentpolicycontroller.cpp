#include "dwmaccentpolicycontroller.h"

DwmAccentPolicyController::DwmAccentPolicyController(QWidget *parent) : QWidget(parent)
{
    m_maskCheckBox = new QCheckBox(this);
    m_maskCheckBox->setChecked(false);
    connect(m_maskCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(requireUpdate()));

    m_customGradient = new QCheckBox(this);
    m_customGradient->setChecked(false);
    connect(m_customGradient, SIGNAL(toggled(bool)), this, SIGNAL(requireUpdate()));

    m_windowFlagsBox = new QComboBox(this);
    m_windowFlagsBox->addItem("Window", int(Qt::Widget));
    m_windowFlagsBox->addItem("ToolTip", int(Qt::ToolTip));
    m_windowFlagsBox->addItem("Tool", int(Qt::Tool));
    m_windowFlagsBox->addItem("Popup", int(Qt::Popup));

    m_windowFlagsBox->setCurrentIndex(0);
    connect(m_windowFlagsBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(requireUpdate()));

    m_accentComboBox = new QComboBox(this);
    m_accentComboBox->addItem("ACCENT_DISABLED", 0);
    m_accentComboBox->addItem("ACCENT_ENABLE_GRADIENT", 1);
    m_accentComboBox->addItem("ACCENT_ENABLE_TRANSPARENTGRADIENT", 2);
    m_accentComboBox->addItem("ACCENT_ENABLE_BLURBEHIND", 3);
    m_accentComboBox->addItem("ACCENT_ENABLE_ACRYLICBLURBEHIND", 4);
    m_accentComboBox->addItem("ACCENT_INVALID_STATE", 5);
    m_accentComboBox->setCurrentIndex(3);
    connect(m_accentComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(requireUpdate()));

    for (int i = 0; i < 16; ++i)
    {
        const int value = 1 << i;
        m_flagsBox[i] = new QCheckBox(this);
        m_flagsBox[i]->setText("0x" + QString::number(value, 16));
        m_flagsBox[i]->setProperty("flagValue", value);
        const bool on = (value == 0x10);

        m_flagsBox[i]->setChecked(on);

        connect(m_flagsBox[i], SIGNAL(toggled(bool)), this, SIGNAL(requireUpdate()));
    }

    m_colorButton = new ColorPreview(this);
    connect(m_colorButton, SIGNAL(colorChanged(QColor)), this, SIGNAL(requireUpdate()));

    QFormLayout* layout = new QFormLayout(this);
    layout->addRow("Window Masked:", m_maskCheckBox);
    layout->addRow("Custom Gradient:", m_customGradient);
    layout->addRow("Window Flags:", m_windowFlagsBox);
    layout->addRow("Accent State:", m_accentComboBox);
    for (int i = 0; i < 16; ++i)
        layout->addRow("Accent Flag:", m_flagsBox[i]);
    layout->addRow("Color:", m_colorButton);
}

int DwmAccentPolicyController::accentState() const
{
    return m_accentComboBox->currentData().toInt();
}

int DwmAccentPolicyController::accentFlags() const
{
    int flags = 0;
    for (int i = 0; i < 16; ++i)
    {
        if (m_flagsBox[i]->isChecked())
            flags |= m_flagsBox[i]->property("flagValue").toInt();
    }
    return flags;
}

QColor DwmAccentPolicyController::accentColor() const
{
    return m_colorButton->color();
}

Qt::WindowFlags DwmAccentPolicyController::selectedWindowFlags() const
{
    return Qt::WindowFlags(m_windowFlagsBox->currentData().toInt());
}

bool DwmAccentPolicyController::isMasked() const
{
    return m_maskCheckBox->isChecked();
}

bool DwmAccentPolicyController::hasCustomGradient() const
{
    return m_customGradient->isChecked();
}


ColorPreview::ColorPreview(QWidget *parent)
    : QWidget(parent)
{
    connect(this, SIGNAL(colorChanged(QColor)), this, SLOT(update()));
    setMaximumSize(32, 32);
}

QColor ColorPreview::color() const
{
    return m_color;
}

void ColorPreview::setColor(QColor c)
{
    if (m_color == c)
        return;

    m_color = c;
    Q_EMIT colorChanged(c);
}

bool ColorPreview::hasHeightForWidth() const
{
    return true;
}

int ColorPreview::heightForWidth(int w) const
{
    return w;
}

void ColorPreview::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton &&
        event->modifiers() == Qt::NoModifier)
    {
        QColor c = QColorDialog::getColor(Qt::white, this);
        if (c.isValid())
            setColor(c);
    }
}

void ColorPreview::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QBrush(m_color));
}
