#ifndef TOOLWINDOW_H
#define TOOLWINDOW_H

#include <QWidget>

class QPropertyAnimation;
class QPushButton;

class ToolWindow : public QWidget
{
    Q_OBJECT

public:
    ToolWindow(QWidget *_parent = 0);

protected:
    void showEvent(QShowEvent* _event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent* _event) Q_DECL_OVERRIDE;

    void paintEvent(QPaintEvent* _event) Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent* _event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* _event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* _event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onAnimationFinished();

private:
    QPoint pos_;
    QPropertyAnimation* animation_;
    QPushButton* button_;
};

#endif // TOOLWINDOW_H
