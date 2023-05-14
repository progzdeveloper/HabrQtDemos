#ifndef DWMACCENTPOLICYCONTROLLER_H
#define DWMACCENTPOLICYCONTROLLER_H

#include <QtWidgets>

class ColorPreview;

class DwmAccentPolicyController : public QWidget
{
    Q_OBJECT
public:
    explicit DwmAccentPolicyController(QWidget *parent = 0);

    int accentState() const;
    int accentFlags() const;
    QColor accentColor() const;
    bool isMasked() const;
    bool hasCustomGradient() const;
    Qt::WindowFlags selectedWindowFlags() const;

Q_SIGNALS:
    void requireUpdate();

private:
    QColor m_accentColor;

    QCheckBox* m_framelessCheckBox;
    QCheckBox* m_maskCheckBox;
    QCheckBox* m_customGradient;
    QComboBox* m_windowFlagsBox;
    QComboBox* m_accentComboBox;
    QCheckBox* m_flagsBox[16];
    ColorPreview* m_colorButton;
};

class ColorPreview : public QWidget
{
    Q_OBJECT
public:
    explicit ColorPreview(QWidget* parent = 0);

    QColor color() const;
    void setColor(QColor c);

    bool hasHeightForWidth() const Q_DECL_OVERRIDE;
    int heightForWidth(int) const Q_DECL_OVERRIDE;

Q_SIGNALS:
    void colorChanged(QColor c);

protected:
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    QColor m_color;
};

#endif // DWMACCENTPOLICYCONTROLLER_H
