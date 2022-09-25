#ifndef BRUSHPREVIEW_H
#define BRUSHPREVIEW_H

#include <QWidget>

class BrushPreview : public QWidget
{
    Q_OBJECT
public:
    explicit BrushPreview(QWidget *parent = Q_NULLPTR);
    void setColorDialogEnabled(bool _on);
    bool isColorDialogEnabled() const;

public Q_SLOTS:
    void updateColor(QColor _color);
    void updateThickness(int _value);

Q_SIGNALS:
    void colorChanged(QColor);

protected:
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* _event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* _event) Q_DECL_OVERRIDE;

private:
    QColor color_;
    int width_;
    bool hasColorDialog_;
    bool pressed_;
};

#endif // BRUSHPREVIEW_H
