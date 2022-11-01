#pragma once
#include <QtWidgets>

class OverlayWidget;
class ControlPanel;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;

private:
    QWidget* container;
    OverlayWidget* overlay;
};


class BlurBehindEffect;
class OverlayWidget : public QWidget
{
    Q_OBJECT

public:
    OverlayWidget(BlurBehindEffect* effect, QWidget* parent = Q_NULLPTR);

protected:
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

private:
    QPointer<BlurBehindEffect> mEffect;
};


class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    ControlPanel(QWidget* parent);

    void setEffect(BlurBehindEffect* e);
    BlurBehindEffect* effect() const;

    void setWidget(OverlayWidget* overlay);
    OverlayWidget* widget() const;

private:
    void initUi();

private Q_SLOTS:
    void updateValues();
    void setupValues();

private:
    BlurBehindEffect* blurEffect;
    OverlayWidget* overlayWidget;
    QComboBox* blurMethodBox;
    QSpinBox* blurRadiusBox;
    QDoubleSpinBox* blurOpacityBox;
    QDoubleSpinBox* sourceOpacityBox;
    QDoubleSpinBox* downsampleBox;
    QSpinBox* rColorBox;
    QSpinBox* gColorBox;
    QSpinBox* bColorBox;
    QSpinBox* aColorBox;
    QCheckBox* brushBox;
    QCheckBox* enabledBox;
};
