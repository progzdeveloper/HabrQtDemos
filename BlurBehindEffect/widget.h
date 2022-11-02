#pragma once
#include <QtWidgets>

class OverlayWidget;
class ControlPanel;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget* _parent = nullptr);
    ~Widget();

protected:
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;

private:
    QWidget* container_;
    OverlayWidget* overlay_;
};


class BlurBehindEffect;
class OverlayWidget : public QWidget
{
    Q_OBJECT

public:
    OverlayWidget(BlurBehindEffect* _effect, QWidget* _parent = nullptr);

protected:
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* _event) Q_DECL_OVERRIDE;

private:
    QPointer<BlurBehindEffect> effect_;
};


class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    ControlPanel(QWidget* _parent);

    void setEffect(BlurBehindEffect* _effect);
    BlurBehindEffect* effect() const;

    void setWidget(OverlayWidget* _overlay);
    OverlayWidget* widget() const;

Q_SIGNALS:
    void closed();

protected:
    void closeEvent(QCloseEvent* _event);

private:
    void initUi();

private Q_SLOTS:
    void updateValues();
    void setupValues();

private:
    BlurBehindEffect* blurEffect_;
    OverlayWidget* overlayWidget_;
    QComboBox* blurMethodBox_;
    QSpinBox* blurRadiusBox_;
    QDoubleSpinBox* blurOpacityBox_;
    QDoubleSpinBox* sourceOpacityBox_;
    QDoubleSpinBox* downsampleBox_;
    QSpinBox* rColorBox_;
    QSpinBox* gColorBox_;
    QSpinBox* bColorBox_;
    QSpinBox* aColorBox_;
    QCheckBox* brushBox_;
    QCheckBox* enabledBox_;
};
