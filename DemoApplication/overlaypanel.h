#pragma once
#include <QWidget>
#include <QPointer>

class BlurBehindEffect;
class OverlayPanel : public QWidget
{
    Q_OBJECT
public:
    explicit OverlayPanel(BlurBehindEffect* _effect, QWidget *_parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* _event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent* _event) Q_DECL_OVERRIDE;

    virtual QPainterPath clipPath() const;

protected:
    QPointer<BlurBehindEffect> effect_;
    QPainterPath cachedPath_;
};
