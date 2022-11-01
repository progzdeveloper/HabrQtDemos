#pragma once
#include <chrono>
#include <QtWidgets>
#include "overlaypanel.h"

class CustomButton;

class PopupPanel : public OverlayPanel
{
    Q_OBJECT
public:
    enum class Category
    {
        WarningPanel,
        MessagePanel
    };

    explicit PopupPanel(Category _category, BlurBehindEffect* _effect, QWidget *_parent = nullptr);

    Category category() const;

    void setText(const QString& _text);
    QString text() const;

    void setDuration(std::chrono::milliseconds _duration);
    std::chrono::milliseconds duration() const;

Q_SIGNALS:
    void closeRequested();

protected:
    void showEvent(QShowEvent* _event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent* _event) Q_DECL_OVERRIDE;
    QPainterPath clipPath() const Q_DECL_OVERRIDE;

private:
    QTimer* timer_;
    QLabel* label_;
    CustomButton* closeButton_;
    Category category_;
};
