#include "toolwindow.h"
#include <QMouseEvent>
#include <QShowEvent>
#include <QCloseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVBoxLayout>

namespace
{
constexpr QPoint kInvalidPoint(-1, -1);
constexpr qreal kWindowOpacity = 0.6;
}

ToolWindow::ToolWindow(QWidget *_parent)
    : QWidget(_parent)
    , pos_(kInvalidPoint)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setMouseTracking(true);
    setCursor(Qt::OpenHandCursor);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(256, 256);

    button_ = new QPushButton(tr("Close"), this);
    button_->setCursor(Qt::ArrowCursor);
    connect(button_, &QPushButton::clicked, this, &ToolWindow::close);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(button_, 0, Qt::AlignCenter);

    animation_ = new QPropertyAnimation(this);
    animation_->setTargetObject(this);
    animation_->setPropertyName("windowOpacity");
    animation_->setStartValue(0.0);
    animation_->setEndValue(1.0);
    animation_->setDuration(500);
    connect(animation_, &QPropertyAnimation::finished, this, &ToolWindow::onAnimationFinished);
    setWindowOpacity(0.0);
}

void ToolWindow::onAnimationFinished()
{
    if (animation_->direction() == QPropertyAnimation::Backward &&
        animation_->state() == QPropertyAnimation::Stopped)
        close();
}

void ToolWindow::showEvent(QShowEvent *_event)
{
    QWidget::showEvent(_event);
    animation_->setDirection(QPropertyAnimation::Forward);
    animation_->start();
}

void ToolWindow::closeEvent(QCloseEvent *_event)
{
    if (animation_->direction() == QPropertyAnimation::Backward &&
        animation_->state() == QPropertyAnimation::Stopped)
        return QWidget::closeEvent(_event);

    animation_->setDirection(QPropertyAnimation::Backward);
    animation_->start();
    _event->ignore();
}

void ToolWindow::paintEvent(QPaintEvent *_event)
{
    QPainter painter(this);
    painter.setOpacity(kWindowOpacity);
    painter.fillRect(_event->rect(), Qt::black);
}

void ToolWindow::mousePressEvent(QMouseEvent *_event)
{
    if (_event->button() == Qt::LeftButton &&
        _event->modifiers() == Qt::NoModifier)
    {
        pos_ = _event->globalPos();
        setCursor(Qt::ClosedHandCursor);
        return;
    }
    QWidget::mousePressEvent(_event);
}

void ToolWindow::mouseMoveEvent(QMouseEvent *_event)
{
    if (pos_ == kInvalidPoint)
        return QWidget::mouseMoveEvent(_event);

    const QPoint delta = _event->globalPos() - pos_;
    move(pos() + delta);
    pos_ = _event->globalPos();
}

void ToolWindow::mouseReleaseEvent(QMouseEvent *_event)
{
    pos_ = kInvalidPoint;
    setCursor(Qt::OpenHandCursor);
    QWidget::mouseReleaseEvent(_event);
}

