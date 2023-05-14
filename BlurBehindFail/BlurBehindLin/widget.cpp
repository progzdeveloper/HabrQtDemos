#include "widget.h"
#include "windowfunctions.h"
#include "xcbwindowmanager.h"
#include "blur.h"
#include <QPainter>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QMouseEvent>

class WindowCompositionHandler : public ExternalWindowHandler
{
public:
    WindowCompositionHandler(QWidget* w)
        : widget(w)
    {
        rootImage = grabRootImage();
        reset();
    }

    void reset()
    {
        wId = leaderWindow(widget->effectiveWinId());
        pixmap = QImage(widget->size(), QImage::Format_ARGB32);

        QPainter painter(&pixmap);
        painter.drawImage(widget->rect(), rootImage, widget->geometry());
    }

    bool processWindow(WId w) Q_DECL_OVERRIDE
    {
        if (w == wId)
            return true;

        if (!isVisible(w))
            return false;

        const QRect wRect = windowGeometry(w);
        QRect intersected = wRect.intersected(widget->geometry());
        if (intersected.isValid())
        {
            QPoint offset = widget->mapFromGlobal(intersected.topLeft());
            intersected.moveTo(intersected.topLeft() - wRect.topLeft());
            QImage image = windowImage(w, intersected);
            intersected.moveTo(offset);
            QPainter painter(&pixmap);
            painter.drawImage(intersected,image);
        }

        return false;
    }

    QPixmap result() const
    {
        const QSize size = pixmap.size();
        const QSize scaledSize = (QSizeF(size) * 0.25).toSize();
        QPixmap p = QPixmap::fromImage(stackBlurImage(pixmap.scaled(scaledSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation), 3, 2));
        return p.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    QImage rootImage;
    QImage pixmap;
    QWidget* widget;
    WId wId;
};


const QPoint kInvalidPoint{-1, -1};

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(windowFlags() | Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

    compositor = new WindowCompositionHandler(this);
    position = kInvalidPoint;
    timer = new QTimer(this);
    timer->setInterval(10);
    connect(timer, &QTimer::timeout, this, &Widget::compose);

    resize(480, 480);
}

Widget::~Widget()
{
    delete compositor;
}

void Widget::compose()
{
    compositor->reset();
    XcbWindowManager::instance().traverse(compositor);
    pixmap = compositor->result();
    update();
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton &&
        event->modifiers() == Qt::NoModifier)
    {
        position = event->globalPos();
        setCursor(Qt::ClosedHandCursor);
        return;
    }
    QWidget::mousePressEvent(event);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (position == kInvalidPoint)
        return QWidget::mouseMoveEvent(event);

    const QPoint delta = event->globalPos() - position;
    move(pos() + delta);
    position = event->globalPos();
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    position = kInvalidPoint;
    setCursor(Qt::OpenHandCursor);
    QWidget::mouseReleaseEvent(event);
}

void Widget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setOpacity(0.8);
    if (!pixmap.isNull())
        painter.drawPixmap(0, 0, pixmap);
}

void Widget::hideEvent(QHideEvent *event)
{
    timer->stop();
    QWidget::hideEvent(event);
}

void Widget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    timer->start();
}

void Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    setMask(QRegion(rect(), QRegion::Ellipse));
}
