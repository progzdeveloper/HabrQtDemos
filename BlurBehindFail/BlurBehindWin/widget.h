#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPoint>

#include <QPointer>

class DwmAccentPolicyController;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    void updateComposition();

private:
    QPoint position_;
    QPointer<DwmAccentPolicyController> ctrl;
};

#endif // WIDGET_H
