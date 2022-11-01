#include "popuppanel.h"
#include "../CustomButton/custombutton.h"

namespace
{
    const QMargins layoutMargins(PopupPanel::Category _category)
    {
        switch(_category)
        {
        case PopupPanel::Category::WarningPanel:
            return QMargins{8, 0, 8, 0};
        case PopupPanel::Category::MessagePanel:
            return QMargins{12, 16, 12, 16};
        default:
            break;
        }
        return QMargins{8, 8, 8, 8};
    }

    constexpr int kLayoutSpacing = 6;
    constexpr qreal kPanelRadius = 8;
    constexpr qreal kPanelOpacity = 0.8;
}

PopupPanel::PopupPanel(Category _category, BlurBehindEffect *_effect, QWidget *_parent)
    : OverlayPanel(_effect, _parent)
    , timer_(Q_NULLPTR)
    , label_(Q_NULLPTR)
    , closeButton_(Q_NULLPTR)
    , category_(_category)
{
    timer_ = new QTimer(this);
    timer_->setSingleShot(true);
    timer_->setInterval(0);
    connect(timer_, &QTimer::timeout, this, &PopupPanel::closeRequested);

    label_ = new QLabel(this);
    label_->setStyleSheet("color: white");

    if (category_ == Category::WarningPanel)
    {
        closeButton_ = new CustomButton(this);
        closeButton_->setCursor(Qt::PointingHandCursor);
        closeButton_->setIcon(QIcon(":/icons/close"));
        closeButton_->setIconSize({28, 28});
        closeButton_->setButtonStyle(Qt::ToolButtonIconOnly);
        closeButton_->setPopupMode(CustomButton::InstantPopup);
        connect(closeButton_, &CustomButton::clicked, this, &PopupPanel::closeRequested);
    }

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(layoutMargins(_category));
    layout->setSpacing(kLayoutSpacing);

    layout->addWidget(label_);
    if (closeButton_)
        layout->addWidget(closeButton_);

    QPalette pal;
    switch(category_)
    {
    case Category::MessagePanel:
        pal.setBrush(QPalette::Window, Qt::black);
        break;
    case Category::WarningPanel:
        pal.setBrush(QPalette::Window, Qt::red);
        break;
    }
    setPalette(pal);
}

PopupPanel::Category PopupPanel::category() const
{
    return category_;
}

void PopupPanel::setText(const QString &_text)
{
    label_->setText(_text);
}

QString PopupPanel::text() const
{
    return label_->text();
}

void PopupPanel::setDuration(std::chrono::milliseconds _duration)
{
    timer_->setInterval((int)_duration.count());
}

std::chrono::milliseconds PopupPanel::duration() const
{
    return std::chrono::milliseconds(timer_->interval());
}

void PopupPanel::showEvent(QShowEvent *_event)
{
    if (timer_->interval() > 0)
        timer_->start();

    QWidget::showEvent(_event);
}


void PopupPanel::paintEvent(QPaintEvent* _event)
{
    /*const QRect r = rect().adjusted(0, 0, -1, -1);
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette().window());
    painter.setOpacity(kPanelOpacity);
    painter.drawRoundedRect(r, kPanelRadius, kPanelRadius);*/


    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette().window());
    painter.setOpacity(kPanelOpacity);
    painter.setClipPath(cachedPath_);
    painter.fillRect(rect(), palette().window());

    OverlayPanel::paintEvent(_event);
}


QPainterPath PopupPanel::clipPath() const
{
    const QRect r = rect().adjusted(0, 0, -1, -1);
    QPainterPath path;
    path.addRoundedRect(r, kPanelRadius, kPanelRadius);
    return path;
}
