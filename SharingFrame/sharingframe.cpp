#include "sharingframe.h"
#include <QScreen>
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QResizeEvent>

SharingFrame::SharingFrame(QScreen* _screen)
    : QFrame(Q_NULLPTR)
{
    // setup window flags - disable system header
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::ToolTip);

    closeButton_ = new QPushButton(this);
    closeButton_->setText("Stop sharing");
    connect(closeButton_, &QPushButton::clicked, qApp, &QApplication::quit);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(QMargins{});
    layout->setSpacing(0);
    layout->addWidget(closeButton_, 0, Qt::AlignTop|Qt::AlignCenter);

    if (!_screen)
        _screen = qApp->primaryScreen();

    if (!_screen)
        hide();
    else
        setGeometry(_screen->geometry());

    setStyleSheet("background: rgb(0, 255, 0); border: 4px solid rgb(0, 255, 0)");
}

void SharingFrame::resizeEvent(QResizeEvent* _event)
{
    updateMask(_event->size());
    QFrame::resizeEvent(_event);
}

void SharingFrame::updateMask(const QSize& _size)
{
    const int w = frameWidth();
    const QRect r(QPoint{}, _size);
    QRegion region(r);
    region -= r.adjusted(w, w, -w, -w);
    region += closeButton_->geometry();
    setMask(region);
}

