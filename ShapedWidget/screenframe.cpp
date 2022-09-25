#include "screenframe.h"
#include <QScreen>
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>

ScreenFrame::ScreenFrame(QScreen* _screen)
    : ShapedWidget()
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
