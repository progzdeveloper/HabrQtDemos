#include "coloredit.h"
#include <QStyle>
#include <QMenu>
#include <QAction>
#include <QSlider>
#include <QLineEdit>
#include <QFormLayout>

ColorEdit::ColorEdit(QWidget *parent) : QWidget(parent)
{
    rSpinBox = createSpinBox();
    labels[0] = new QLabel(tr("Red"), this);

    gSpinBox = createSpinBox();
    labels[1] = new QLabel(tr("Green"), this);

    bSpinBox = createSpinBox();
    labels[2] = new QLabel(tr("Blue"), this);

    aSpinBox = createSpinBox();
    labels[3] = new QLabel(tr("Alpha"), this);

    QFormLayout* layout = new QFormLayout(this);
    layout->addRow(labels[0], rSpinBox);
    layout->addRow(labels[1], gSpinBox);
    layout->addRow(labels[2], bSpinBox);
    layout->addRow(labels[3], aSpinBox);
}

void ColorEdit::setChannels(Channels _channels)
{
    channels_ = _channels;

    rSpinBox->setVisible(channels_ & Red);
    labels[0]->setVisible(channels_ & Red);

    gSpinBox->setVisible(channels_ & Green);
    labels[1]->setVisible(channels_ & Green);

    bSpinBox->setVisible(channels_ & Blue);
    labels[2]->setVisible(channels_ & Blue);

    aSpinBox->setVisible(channels_ & Alpha);
    labels[3]->setVisible(channels_ & Alpha);
}

void ColorEdit::setColor(QColor _color)
{
    if (color_ == _color)
        return;

    color_ = _color;

    QSignalBlocker blocker(this);
    rSpinBox->setValue(_color.red());
    gSpinBox->setValue(_color.green());
    bSpinBox->setValue(_color.blue());
    aSpinBox->setValue(_color.alpha());
}

void ColorEdit::updateColor()
{
    if (channels_ & Red)
        color_.setRed(rSpinBox->value());
    if (channels_ & Green)
        color_.setGreen(gSpinBox->value());
    if (channels_ & Blue)
        color_.setBlue(bSpinBox->value());
    if (channels_ & Alpha)
        color_.setAlpha(aSpinBox->value());
    Q_EMIT colorChanged(color_);
}

QSpinBox *ColorEdit::createSpinBox()
{
    QSpinBox* spinBox = new QSpinBox(this);
    connect(spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ColorEdit::updateColor);
    spinBox->setMinimumWidth(78);
    spinBox->setRange(0, 255);
    QLineEdit* lineEdit = spinBox->findChild<QLineEdit*>();
    if (!lineEdit)
        return spinBox;

    QMenu* menu = new QMenu(spinBox);

    QSlider* slider = new QSlider(Qt::Horizontal, menu);
    slider->setRange(spinBox->minimum(), spinBox->maximum());
    connect(slider, &QSlider::valueChanged, spinBox, &QSpinBox::setValue);

    QHBoxLayout* layout = new QHBoxLayout(menu);
    layout->setMargin(1);
    layout->addWidget(slider, 1);

    QAction* action = lineEdit->addAction(style()->standardIcon(QStyle::SP_ArrowDown), QLineEdit::LeadingPosition);
    connect(action, &QAction::triggered, spinBox, [menu, slider, spinBox]()
    {
        if (menu && slider)
        {
            slider->setRange(spinBox->minimum(), spinBox->maximum());
            slider->setValue(spinBox->value());
            menu->setMinimumWidth(spinBox->width());
            menu->exec(spinBox->mapToGlobal(spinBox->rect().bottomLeft()));
        }
    });

    return spinBox;
}
