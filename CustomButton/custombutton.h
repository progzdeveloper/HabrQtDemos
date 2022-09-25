#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

#include <QPointer>
#include <QMenu>
#include <QAbstractButton>
#include <QStyle>

class CustomButton : public QAbstractButton
{
    Q_OBJECT
public:
    enum PopupMode
    {
        InstantPopup,
        MenuButtonPopup
    };

    explicit CustomButton(QWidget* _parent = Q_NULLPTR);
    explicit CustomButton(const QString &_text, QWidget* _parent = Q_NULLPTR);

    void setIconSize(const QSize& _iconSize);
    void setText(const QString &_text);

    void setBadgeValue(const QVariant& _value);
    QVariant badgeValue() const;

    void setButtonStyle(Qt::ToolButtonStyle _style);
    Qt::ToolButtonStyle buttonStyle() const;

    void setPopupMode(PopupMode _mode);
    PopupMode popupMode() const;

    void setAutoRaise(bool _on);
    bool autoRaise() const;

    void setMenu(QMenu* _menu);
    QMenu* menu() const;

    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

protected:
    bool event(QEvent *_event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* _event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;

    virtual void showMenu(QMenu* _menu);

Q_SIGNALS:
    void menuRequested();

private:
    QRect iconRect() const;
    QRect textRect(const QRect& _iconRect) const;
    QRect menuButtonRect(const QRect& _iconRect) const;
    QRect badgeRect(const QRect& _iconRect) const;

    void initStyleOption(QStyleOptionComplex& option);
    void drawBackground(QPainter& _painter, const QStyleOptionComplex& _option, const QRect& _iconRect);
    void drawBadge(QPainter& _painter, const QStyleOptionComplex& _option);
    void drawMenuButton(QPainter& _painter, const QStyleOptionComplex& _option);
    void drawIcon(QPainter& painter, const QIcon& icon, const QStyleOption &option);
    void drawText(QPainter& painter, const QStyleOptionComplex& option);

    void updateHoverControl(const QPoint& _pos);



private:
    mutable QSize sizeHint_;
    QVariant badgeValue_;
    QPointer<QMenu> menu_;
    QStyle::SubControls hoverControl_;
    Qt::ToolButtonStyle buttonStyle_;
    PopupMode popupMode_;
    bool autoRaise_;
};

#endif // CUSTOMBUTTON_H
