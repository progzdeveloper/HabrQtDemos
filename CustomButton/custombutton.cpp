#include "custombutton.h"
#include <algorithm>
#include <QtMath>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionComplex>
#include <QApplication>
#include <QPixmap>

namespace
{
    constexpr int kBorderWidth = 2;
    constexpr int kSpacing = 8;
    constexpr qreal kMenuButtonAngle = qDegreesToRadians(qreal(42));
    constexpr qreal kBadgeAngle = kMenuButtonAngle;
    constexpr QMargins kBorderMargins{kBorderWidth, kBorderWidth, kBorderWidth, kBorderWidth};
    constexpr QMargins kButtonMargins{4, 8, 4, 4};
    constexpr QMargins kIconMargins{8, 8, 8, 8};
    constexpr QSize kBadgeSize{12, 12};
    constexpr QSize kMenuButtonSize{14, 14};

    const QFont badgeFont()
    {
        static QFont font = []()
        {
            QFont f = qApp->font();
            f.setPixelSize(8);
            return f;
        }();
        return font;
    }

    QString bageText(qlonglong _value)
    {
        if (_value < 1)
            return QString{};
        if (_value > 999)
            return QLatin1Literal("1k");
        return QString::number(_value);
    }

    QString bageText(qulonglong _value)
    {
        if (_value <= 1)
            return QString{};
        if (_value > 999)
            return QLatin1Literal("1k");
        return QString::number(_value);
    }

    QString bageText(const QString& _value)
    {
        return _value.isEmpty() ? QString{} : QString(_value[0]);
    }

    QString bageText(const QChar _value)
    {
        return QString{_value};
    }

    bool hasBadge(const QVariant& _value)
    {
        switch(_value.type())
        {
        case QVariant::Brush:
        case QVariant::Color:
            return true;
        case QVariant::Int:
        case QVariant::LongLong:
            return _value.toLongLong() > 0;
        case QVariant::UInt:
        case QVariant::ULongLong:
            return _value.toULongLong() > 0;
        case QVariant::String:
            return !_value.toString().isEmpty();
        default:
            break;
        }
        return false;
    }

    constexpr int spacing() noexcept { return kSpacing; }
    const int spacing(const QSize& _textSize) noexcept { return (_textSize.height() > 0 ? spacing() : 0); }

    QSize buttonMinSize(Qt::ToolButtonStyle _buttonStyle) noexcept
    {
        switch(_buttonStyle)
        {
        case Qt::ToolButtonTextUnderIcon:
            return QSize(22, 44);
        case Qt::ToolButtonIconOnly:
            return QSize(22, 22);
        default:
            break;
        }
        return QSize(44, 22);
    }

    QSize textSize(const QString& _text, const QFontMetrics& _metrcis)
    {
        return _metrcis.size(0, _text);
    }

    QSize textSize(const QString &_text, const QFont &_font)
    {
        return _text.isEmpty() ? QSize{} : textSize(_text, QFontMetrics(_font));
    }

    QColor buttonColor(const QPalette& _palette, QStyle::State _state) noexcept
    {
        const bool enabled = _state & QStyle::State_Enabled;
        const bool pressed = _state & QStyle::State_Sunken;
        const bool hovered = _state & QStyle::State_MouseOver;
        const bool checked = _state & QStyle::State_On;

        QPalette::ColorGroup group = enabled && checked ? QPalette::Normal : QPalette::Disabled;
        QColor color;
        if ((enabled && pressed) || checked)
            color = _palette.color(group, QPalette::Light);
        else if (enabled && hovered)
            color = _palette.color(group, QPalette::Midlight).darker(120);
        else
            color = _palette.color(group, QPalette::Dark).darker(150);

        color.setAlpha(enabled ? 200 : 128);
        return color;
    }

    QColor iconColor(QStyle::State _state)
    {
        const bool enabled = _state & QStyle::State_Enabled;
        const bool pressed = _state & QStyle::State_Sunken;
        const bool checked = _state & QStyle::State_On;
        return enabled && (pressed || checked) ? Qt::black : Qt::white;
    }
}

CustomButton::CustomButton(QWidget* _parent)
    : CustomButton(QString{}, _parent)
{
}

CustomButton::CustomButton(const QString& _text, QWidget* _parent)
    : QAbstractButton(_parent)
    , hoverControl_(QStyle::SC_None)
    , buttonStyle_(Qt::ToolButtonTextUnderIcon)
    , popupMode_(MenuButtonPopup)
    , autoRaise_(false)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::ToolButton));
    setText(_text);
    setIconSize({38, 38});

    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);
    setCheckable(false);
    setChecked(false);
    setContentsMargins(kButtonMargins);
}

void CustomButton::setIconSize(const QSize &_iconSize)
{
    if (iconSize() == _iconSize)
        return;

    sizeHint_ = QSize();
    QAbstractButton::setIconSize(_iconSize);
}

void CustomButton::setText(const QString &_text)
{
    if (text() == _text)
        return;

    sizeHint_ = QSize();
    QAbstractButton::setText(_text);
    update();
}

void CustomButton::setBadgeValue(const QVariant &_value)
{
    if (badgeValue_ == _value)
        return;

    badgeValue_ = _value;
    update();
}

QVariant CustomButton::badgeValue() const
{
    return badgeValue_;
}

void CustomButton::setButtonStyle(Qt::ToolButtonStyle _style)
{
    if (buttonStyle_ == _style)
        return;

    buttonStyle_ = _style;
    sizeHint_ = QSize();
    updateGeometry();
    update();
}

Qt::ToolButtonStyle CustomButton::buttonStyle() const
{
    return buttonStyle_;
}

void CustomButton::setPopupMode(CustomButton::PopupMode _mode)
{
    if (popupMode_ == _mode)
        return;

    popupMode_ = _mode;
    sizeHint_ = QSize();
    update();
}

CustomButton::PopupMode CustomButton::popupMode() const
{
    return popupMode_;
}

void CustomButton::setAutoRaise(bool _on)
{
    if (autoRaise_ == _on)
        return;

    autoRaise_ = _on;
    update();
}

bool CustomButton::autoRaise() const
{
    return autoRaise_;
}

void CustomButton::setMenu(QMenu* _menu)
{
    if (menu_ == _menu)
        return;

    menu_ = _menu;
    sizeHint_ = QSize();
    updateGeometry();
    update();
}

QMenu *CustomButton::menu() const
{
    return menu_;
}

QSize CustomButton::sizeHint() const
{
    if (sizeHint_.isValid())
        return sizeHint_;

    const QRect iconRc(QPoint{}, iconSize());
    const QSize textSz = textSize(text(), font());
    int w = 0, h = 0;

    if (buttonStyle_ == Qt::ToolButtonIconOnly)
    {
        QRect r = iconRc;
        if (popupMode_ == MenuButtonPopup && menu_)
            r |= menuButtonRect(iconRc);
        r |= badgeRect(iconRc);
        w = r.width();
        h = r.height();
    }
    else if (buttonStyle_ == Qt::ToolButtonTextOnly)
    {
        w = textSz.width() + spacing(textSz) + (kIconMargins.left() + kIconMargins.right()) / 2;
        h = textSz.height() + kBadgeSize.height() / 2;
        if (popupMode_ == MenuButtonPopup && menu_)
        {
            w += kMenuButtonSize.width() / 2 + 2;
            h = std::max(h, kMenuButtonSize.height() + 2);
        }
    }
    else if (buttonStyle_ == Qt::ToolButtonTextUnderIcon)
    {
        QRect r = iconRc;
        if (popupMode_ == MenuButtonPopup && menu_)
            r |= menuButtonRect(iconRc);
        r |= badgeRect(iconRc);
        w = std::max(textSz.width(), r.width());
        h = r.height() + spacing(textSz) + textSz.height();
    }
    else if (buttonStyle_ == Qt::ToolButtonTextBesideIcon)
    {
        w = iconRc.width() + spacing(textSz) + textSz.width();
        if (popupMode_ == MenuButtonPopup && menu_)
            w += (spacing() + menuButtonRect(iconRc).width());

        h = std::max(iconRc.height(), textSz.height()) + kBadgeSize.height() / 2;
    }

    const QMargins margins = contentsMargins();
    w += margins.left() + margins.right();
    h += margins.top() + margins.bottom();

    sizeHint_ = QSize(w, h);
    sizeHint_ = sizeHint_.expandedTo(buttonMinSize(buttonStyle_));
    return sizeHint_;
}

QSize CustomButton::minimumSizeHint() const
{
    return sizeHint();
}

bool CustomButton::event(QEvent* _event)
{
    switch(_event->type())
    {
    case QEvent::HoverLeave:
    case QEvent::HoverEnter:
    case QEvent::HoverMove:
        updateHoverControl(static_cast<const QHoverEvent*>(_event)->pos());
        break;
    case QEvent::PaletteChange:
        update();
        break;
    case QEvent::LocaleChange:
    case QEvent::StyleChange:
    case QEvent::LanguageChange:
    case QEvent::FontChange:
        sizeHint_ = QSize();
        break;
    default:
        break;
    }
    return QAbstractButton::event(_event);
}

void CustomButton::mousePressEvent(QMouseEvent* _event)
{
    if ((_event->button() == Qt::LeftButton) &&
        (popupMode_ == InstantPopup ||
         menuButtonRect(iconRect()).contains(_event->pos())))
    {
        setDown(true);
        repaint();
        if (menu_)
        {
            showMenu(menu_);
            setDown(false);
            repaint();
        }
        else if (popupMode_ == MenuButtonPopup)
        {
            Q_EMIT menuRequested();
            setDown(false);
            repaint();
        }

        return;
    }
    QAbstractButton::mousePressEvent(_event);
}

void CustomButton::paintEvent(QPaintEvent*)
{
    QStyleOptionComplex option;
    initStyleOption(option);

    const QStyle::State state = option.state;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect iconRc = iconRect();
    const QRect textRc = textRect(iconRc);

    if (isDown() && (option.activeSubControls & QStyle::SC_ToolButtonMenu))
        option.state &= ~(QStyle::State_Sunken);

    if (buttonStyle_ != Qt::ToolButtonTextOnly)
    {
        drawBackground(painter, option, iconRc);
        option.rect = iconRc.marginsRemoved(kIconMargins);
        drawIcon(painter, icon(), option);
    }
    else
    {
        option.rect = textRc.marginsAdded(kIconMargins);
        drawBackground(painter, option, QRect{});
    }

    option.rect = badgeRect(iconRc);
    drawBadge(painter, option);

    if (popupMode_ != InstantPopup)
    {
        option.rect = menuButtonRect(iconRc);
        option.state &= ~QStyle::State_On;
        if (isDown() && (option.activeSubControls & QStyle::SC_ToolButtonMenu))
            option.state |= QStyle::State_Sunken;
        else
            option.state &= ~(QStyle::State_Sunken);
        drawMenuButton(painter, option);
    }

    if (buttonStyle_ != Qt::ToolButtonIconOnly)
    {
        option.rect = textRc;
        option.state = state;
        drawText(painter, option);
    }
}

void CustomButton::showMenu(QMenu *_menu)
{
    if (!_menu)
        return;

    const QSize s = _menu->sizeHint();
    const QPoint p(width() / 2 - s.width() / 2, -s.height());
    _menu->exec(mapToGlobal(p));
}

void CustomButton::initStyleOption(QStyleOptionComplex &_option)
{
    _option.initFrom(this);

    if (isDown())
        _option.state |= QStyle::State_Sunken;
    if (isChecked())
        _option.state |= QStyle::State_On;
    if (isChecked() && isDown())
        _option.state |= QStyle::State_Raised;

    _option.activeSubControls = hoverControl_;
}

QRect CustomButton::iconRect() const
{
    const QMargins margins = contentsMargins();
    switch(buttonStyle_)
    {
    case Qt::ToolButtonIconOnly:
    case Qt::ToolButtonTextUnderIcon:
        return QRect(QPoint{}, iconSize()).translated(width() / 2 - iconSize().width() / 2, margins.top());
    case Qt::ToolButtonTextBesideIcon:
    default:
        return QRect(QPoint{}, iconSize()).translated(margins.left(), margins.top());
    }
    return QRect{};
}

QRect CustomButton::textRect(const QRect &_iconRect) const
{
    if (buttonStyle_ == Qt::ToolButtonIconOnly)
        return QRect{};

    const QSize textSz = textSize(text(), font());
    QRect r{QPoint{}, textSz};
    switch(buttonStyle_)
    {
    case Qt::ToolButtonTextOnly:
        r.moveCenter(rect().center());
        break;
    case Qt::ToolButtonTextBesideIcon:
        r.moveLeft(_iconRect.right() + spacing());
        r.moveTop(height() / 2 - textSz.height() / 2);
        break;
    case Qt::ToolButtonTextUnderIcon:
        r.moveLeft(_iconRect.center().x() - r.width() / 2);
        r.moveTop(_iconRect.bottom() + spacing() + 1);
        break;
    default:
        return QRect{};
    }
    return r;
}

QRect CustomButton::badgeRect(const QRect& _iconRect) const
{
    QRect rc{QPoint{}, kBadgeSize};
    if (buttonStyle_ == Qt::ToolButtonTextOnly)
    {
        rc.moveTopRight(rect().topRight() + QPoint(-1, 0));
        return rc;
    }

    const int r = _iconRect.width() / 2 + kBorderWidth;
    const QPoint center{ int(r * (qFastCos(kBadgeAngle))), int(-r * (qFastSin(kBadgeAngle))) };
    rc.moveCenter(_iconRect.center() + center);
    return rc;
}

QRect CustomButton::menuButtonRect(const QRect& _iconRect) const
{
    QRect rc{QPoint{}, kMenuButtonSize};
    if (buttonStyle_ == Qt::ToolButtonTextOnly)
    {
        rc.moveBottomRight(rect().bottomRight());
        return rc;
    }

    const int r = _iconRect.width() / 2 + kBorderWidth / 2;
    const QPoint center{ int(r * (qFastCos(kMenuButtonAngle))), int(r * (qFastSin(kMenuButtonAngle))) };
    rc.moveCenter(_iconRect.center() + center);
    return rc;
}

void CustomButton::drawBackground(QPainter& _painter, const QStyleOptionComplex& _option, const QRect& _iconRect)
{
    const bool checked = _option.state & QStyle::State_On;
    const bool hovered = _option.state & QStyle::State_MouseOver;

    const QSize sz = size();
    if (sz.isEmpty() || (autoRaise_ && !checked && !hovered))
        return;

    QStyle::State state = _option.state;
    if (!(_option.activeSubControls & QStyle::SC_ToolButton))
        state &= ~(QStyle::State_MouseOver | QStyle::State_Sunken);

    const qreal dpr = devicePixelRatioF();
    QPixmap buttonMask((QSizeF(sz) * dpr).toSize());
    buttonMask.setDevicePixelRatio(dpr);
    buttonMask.fill(Qt::transparent);

    QPainter maskPainter(&buttonMask);
    maskPainter.setRenderHint(QPainter::Antialiasing);
    if (buttonStyle_ == Qt::ToolButtonTextOnly)
    {
        const int r = std::min(_option.rect.width(), _option.rect.height()) / 2;
        maskPainter.setPen(Qt::NoPen);
        maskPainter.setBrush(buttonColor(_option.palette, state));
        maskPainter.drawRoundedRect(_option.rect, r, r);
    }
    else
    {
        maskPainter.setPen(Qt::NoPen);
        maskPainter.setBrush(buttonColor(_option.palette, state));
        maskPainter.drawEllipse(_iconRect);
    }

    maskPainter.setCompositionMode(QPainter::CompositionMode_Source);
    maskPainter.setBrush(Qt::transparent);
    if (hasBadge(badgeValue_))
        maskPainter.drawEllipse(badgeRect(_iconRect).marginsAdded(kBorderMargins));
    if (popupMode_ != InstantPopup)
        maskPainter.drawEllipse(menuButtonRect(_iconRect).marginsAdded(kBorderMargins));

    _painter.drawPixmap(0, 0, buttonMask);
}


void CustomButton::drawMenuButton(QPainter& _painter, const QStyleOptionComplex& _option)
{
    _painter.setPen(Qt::NoPen);
    _painter.setBrush(buttonColor(_option.palette, _option.state));
    _painter.drawEllipse(_option.rect);

    static QIcon icon(":/icons/more-horizontal");
    drawIcon(_painter, icon, _option);
}

void CustomButton::drawBadge(QPainter &_painter, const QStyleOptionComplex &_option)
{
    if (!badgeValue_.isValid())
        return;

    QString text;
    QBrush brush;
    switch(badgeValue_.type())
    {
    case QVariant::Color:
        brush = badgeValue_.value<QColor>();
        break;
    case QVariant::Brush:
        brush = badgeValue_.value<QBrush>();
        break;
    case QVariant::UInt:
    case QVariant::ULongLong:
        brush = _option.palette.highlight();
        text = bageText(badgeValue_.toULongLong());
        break;
    case QVariant::Int:
    case QVariant::LongLong:
        brush = _option.palette.highlight();
        text = bageText(badgeValue_.toLongLong());
        break;
    case QVariant::Char:
        brush = _option.palette.highlight();
        text = bageText(badgeValue_.toChar());
        break;
    case QVariant::String:
        brush = _option.palette.highlight();
        text = bageText(badgeValue_.toString());
        break;
    default:
        return;
    }

    _painter.setPen(Qt::NoPen);
    _painter.setBrush(brush);
    _painter.drawEllipse(_option.rect);

    if (!text.isEmpty())
    {
        _painter.setBrush(Qt::NoBrush);
        _painter.setPen(_option.palette.color(QPalette::HighlightedText));
        _painter.setFont(badgeFont());
        _painter.drawText(_option.rect, Qt::AlignCenter, text);
    }
}

void CustomButton::drawIcon(QPainter &painter, const QIcon& icon, const QStyleOption &option)
{
    const QRect r = option.rect;

    QPixmap pixmap(r.size());
    pixmap.fill(Qt::transparent);

    QPainter pixmapPainter(&pixmap);
    icon.paint(&pixmapPainter, pixmap.rect());
    pixmapPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    pixmapPainter.fillRect(pixmap.rect(), iconColor(option.state));

    painter.drawPixmap(r, pixmap);
}

void CustomButton::drawText(QPainter &painter, const QStyleOptionComplex &option)
{
    const bool inverseColor = buttonStyle_ == Qt::ToolButtonTextOnly && (option.state & (QStyle::State_Sunken | QStyle::State_On));
    painter.setFont(font());
    painter.setBrush(Qt::NoBrush);
    painter.setPen(option.palette.color(inverseColor ? QPalette::ButtonText : QPalette::BrightText));
    painter.drawText(option.rect, Qt::AlignCenter, text());
}

void CustomButton::updateHoverControl(const QPoint &_pos)
{
    const QStyle::SubControls lastControl = hoverControl_;
    hoverControl_ = QStyle::SC_None;
    if (rect().contains(_pos))
    {
        hoverControl_ |= QStyle::SC_ToolButton;
        if (popupMode_ != InstantPopup && menuButtonRect(iconRect()).contains(_pos))
            hoverControl_ |= QStyle::SC_ToolButtonMenu;
    }

    if (hoverControl_ != lastControl)
        update();
}

