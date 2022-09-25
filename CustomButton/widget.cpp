#include "widget.h"
#include "custombutton.h"
#include <QPainter>
#include <QtWidgets>

namespace
{
    const QColor kBackgroundColor(84, 84, 84);
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("QLabel{color:white}");

    QMenu* menu = new QMenu;
    menu->addAction(tr("Select color"), this, &Widget::selectColor);
    menu->addAction(tr("Setup counter"), this, &Widget::setupCounter);
    menu->addAction(tr("Reset badge"), this, &Widget::resetBagde);

    button = new CustomButton(this);
    button->setText(tr("Pencil"));
    button->setMenu(menu);
    button->setAutoRaise(false);
    button->setBadgeValue(QColor(0, 255, 0));
    button->setIcon(QIcon(":/icons/pencil"));

    QCheckBox* enabledChecker = new QCheckBox(this);
    enabledChecker->setChecked(button->isEnabled());
    connect(enabledChecker, &QCheckBox::toggled, button, &CustomButton::setEnabled);

    QCheckBox* autoRaiseChecker = new QCheckBox(this);
    autoRaiseChecker->setChecked(button->autoRaise());
    connect(autoRaiseChecker, &QCheckBox::toggled, button, &CustomButton::setAutoRaise);

    QCheckBox* checkableChecker = new QCheckBox(this);
    checkableChecker->setChecked(button->isCheckable());
    connect(checkableChecker, &QCheckBox::toggled, button, &CustomButton::setCheckable);

    typedef void(QComboBox::* cbIndexChanged)(int);
    QComboBox* styleComboBox = new QComboBox(this);
    styleComboBox->addItem("Icon Only", (int)Qt::ToolButtonIconOnly);
    styleComboBox->addItem("Text Only", (int)Qt::ToolButtonTextOnly);
    styleComboBox->addItem("Text Under Icon", (int)Qt::ToolButtonTextUnderIcon);
    styleComboBox->addItem("Text Beside Icon", (int)Qt::ToolButtonTextBesideIcon);
    styleComboBox->setCurrentIndex(styleComboBox->findData((int)button->buttonStyle()));
    connect(styleComboBox, static_cast<cbIndexChanged>(&QComboBox::currentIndexChanged), this, [this, styleComboBox](int i)
    {
        button->setButtonStyle((Qt::ToolButtonStyle)styleComboBox->itemData(i).toInt());
    });

    QComboBox* popupComboBox = new QComboBox(this);
    popupComboBox->addItem("Menu Button Popup", (int)CustomButton::MenuButtonPopup);
    popupComboBox->addItem("Instant Menu Popup", (int)CustomButton::InstantPopup);
    popupComboBox->setCurrentIndex(popupComboBox->findData((int)button->popupMode()));
    connect(popupComboBox, static_cast<cbIndexChanged>(&QComboBox::currentIndexChanged), this, [this, popupComboBox](int i)
    {
        button->setPopupMode((CustomButton::PopupMode)popupComboBox->itemData(i).toInt());
    });

    QFormLayout* controlsLayout = new QFormLayout;
    controlsLayout->addRow(tr("Enabled:"), enabledChecker);
    controlsLayout->addRow(tr("Auto-raise:"), autoRaiseChecker);
    controlsLayout->addRow(tr("Checkable:"), checkableChecker);
    controlsLayout->addRow(tr("Button style:"), styleComboBox);
    controlsLayout->addRow(tr("Menu popup:"), popupComboBox);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(button, 0, Qt::AlignCenter);
    layout->addLayout(controlsLayout);

    setMinimumWidth(346);
}

Widget::~Widget()
{
}

void Widget::selectColor()
{
    QColor color = QColorDialog::getColor(button->badgeValue().value<QColor>(), this, tr("Select color"));
    if (color.isValid())
        button->setBadgeValue(color);
}

void Widget::setupCounter()
{
    bool result = false;
    int counter = QInputDialog::getInt(this, tr("Setup counter"), tr("Counter:"), 1, 0, INT_MAX, 1, &result);
    if (result)
        button->setBadgeValue(counter);
}

void Widget::resetBagde()
{
    button->setBadgeValue(QVariant{});
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), kBackgroundColor);
}
