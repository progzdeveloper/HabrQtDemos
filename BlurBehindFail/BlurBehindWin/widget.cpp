#include "widget.h"
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>

#include <QAction>

#include <QRegion>
#include <QPainterPath>

#include <QMouseEvent>
#include <QKeyEvent>

#include "dwmaccentpolicycontroller.h"

#ifdef Q_OS_WIN
#include <QDebug>
#include <QtWinExtras/QtWin>

#include <QSysInfo>

#include <windows.h>

#define RGBA(r,g,b,a)  ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)|(((DWORD)(BYTE)(a))<<24)))

typedef enum _WINDOWCOMPOSITIONATTRIB
{
    WCA_UNDEFINED = 0,
    WCA_NCRENDERING_ENABLED = 1,
    WCA_NCRENDERING_POLICY = 2,
    WCA_TRANSITIONS_FORCEDISABLED = 3,
    WCA_ALLOW_NCPAINT = 4,
    WCA_CAPTION_BUTTON_BOUNDS = 5,
    WCA_NONCLIENT_RTL_LAYOUT = 6,
    WCA_FORCE_ICONIC_REPRESENTATION = 7,
    WCA_EXTENDED_FRAME_BOUNDS = 8,
    WCA_HAS_ICONIC_BITMAP = 9,
    WCA_THEME_ATTRIBUTES = 10,
    WCA_NCRENDERING_EXILED = 11,
    WCA_NCADORNMENTINFO = 12,
    WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
    WCA_VIDEO_OVERLAY_ACTIVE = 14,
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
    WCA_DISALLOW_PEEK = 16,
    WCA_CLOAK = 17,
    WCA_CLOAKED = 18,
    WCA_ACCENT_POLICY = 19,
    WCA_FREEZE_REPRESENTATION = 20,
    WCA_EVER_UNCLOAKED = 21,
    WCA_VISUAL_OWNER = 22,
    WCA_LAST = 23
} WINDOWCOMPOSITIONATTRIB;

typedef enum {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5,
    _ACCENT_STATE_SIZE = 0xFFFFFFFF
} ACCENT_STATE;

typedef struct {
    ACCENT_STATE accentState;
    DWORD accentFlags;
    COLORREF gradientColor;
    DWORD animationId;
} DWMACCENTPOLICY;

typedef struct _WINCOMPATTR_DATA {
    DWMACCENTPOLICY accentPolicy;
} WINCOMPATTR_DATA;

typedef struct tagWINCOMPATTR
{
    DWORD attribute; // the attribute to query
    WINCOMPATTR_DATA* pData; // buffer to store the result
    ULONG dataSize; // size of the pData buffer
} WINCOMPATTR;
#endif

void enableBlurBehind(QWidget* _widget)
{
#ifdef Q_OS_WIN
    if (QSysInfo::windowsVersion() < QSysInfo::WV_WINDOWS7)
    {
        qCritical() << "Blur Behind effect is not supported on that platform";
        return;
    }

    if (QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS7)
    {
        QtWin::enableBlurBehindWindow(_widget);
    }
    else
    {
        typedef HRESULT(WINAPI* pfnSetWindowCompositionAttribute)(HWND, WINCOMPATTR*);
        pfnSetWindowCompositionAttribute pSetWindowCompositionAttribute = 0;

        HMODULE user32 = ::LoadLibraryA("user32.dll");
        if (user32)
        {
            pSetWindowCompositionAttribute =
                (pfnSetWindowCompositionAttribute)GetProcAddress(user32, "SetWindowCompositionAttribute");
        }
        DWORD dwFlags = 0x10;
        DWMACCENTPOLICY policy = { ACCENT_ENABLE_BLURBEHIND, dwFlags, 0, 0};
        WINCOMPATTR data = { WCA_ACCENT_POLICY, (WINCOMPATTR_DATA*)&policy, sizeof(DWMACCENTPOLICY) };

        if (pSetWindowCompositionAttribute)
            pSetWindowCompositionAttribute((HWND)_widget->winId(), &data);

        ::FreeLibrary(user32);
    }
#endif
}



void enableBlurBehindExtended(QWidget *_widget, DwmAccentPolicyController* _ctrl)
{
#ifdef Q_OS_WIN
    if (QSysInfo::windowsVersion() < QSysInfo::WV_WINDOWS7)
    {
        qCritical() << "Blur Behind effect is not supported on that platform";
        return;
    }

    if (QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS7)
    {
        QtWin::enableBlurBehindWindow(_widget);
    }
    else
    {
        typedef HRESULT(WINAPI* pfnSetWindowCompositionAttribute)(HWND, WINCOMPATTR*);
        pfnSetWindowCompositionAttribute pSetWindowCompositionAttribute = 0;

        HMODULE user32 = ::LoadLibraryA("user32.dll");
        if (user32)
        {
            pSetWindowCompositionAttribute =
                (pfnSetWindowCompositionAttribute)GetProcAddress(user32, "SetWindowCompositionAttribute");
        }

        const QColor c = _ctrl->accentColor().toRgb();
        COLORREF color = RGBA(c.red(), c.green(), c.blue(), 127);
        DWMACCENTPOLICY policy = { (ACCENT_STATE)_ctrl->accentState(), _ctrl->accentFlags(), color, 0 };
        WINCOMPATTR data = { WCA_ACCENT_POLICY, (WINCOMPATTR_DATA*)&policy, sizeof(DWMACCENTPOLICY) };
        if (pSetWindowCompositionAttribute)
            pSetWindowCompositionAttribute((HWND)_widget->winId(), &data);

        ::FreeLibrary(user32);
    }
#endif
}


const QPoint kInvalidPoint( -1, -1 );

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    position_ = QPoint(-1, -1 );

    setAttribute(Qt::WA_TranslucentBackground);
    //enableBlurBehind(this);

    QAction* action = new QAction(this);
    action->setText(tr("Close"));
    connect(action, &QAction::triggered, qApp, &QApplication::quit);

    addAction(action);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    ctrl = new DwmAccentPolicyController;
    connect(ctrl, &DwmAccentPolicyController::requireUpdate, this, &Widget::updateComposition);

    ctrl->show();
    updateComposition();
}

Widget::~Widget()
{
}

void Widget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton &&
        event->modifiers() == Qt::NoModifier)
    {
        position_ = event->globalPos();
        setCursor(Qt::ClosedHandCursor);
    }
    QWidget::mousePressEvent(event);
}

void Widget::mouseMoveEvent(QMouseEvent* event)
{
    if (position_ == kInvalidPoint)
        return QWidget::mouseMoveEvent(event);

    const QPoint delta = event->globalPos() - position_;
    move(pos() + delta);
    position_ = event->globalPos();
}

void Widget::mouseReleaseEvent(QMouseEvent* event)
{
    position_ = kInvalidPoint;
    setCursor(Qt::OpenHandCursor);
    QWidget::mouseReleaseEvent(event);
}

void Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (ctrl->isMasked())
        setMask(QRegion(rect(), QRegion::Ellipse));
    else
        clearMask();
}

void Widget::paintEvent(QPaintEvent *event)
{
    if (!ctrl->hasCustomGradient())
        return;
    QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
    gradient.setColorAt(0.0, QColor(255, 255, 255, 196));
    gradient.setColorAt(1.0, QColor(255, 255, 255, 0));
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRect(rect());
}

void Widget::updateComposition()
{
    if (ctrl)
    {
        if (ctrl->isMasked())
            setMask(QRegion(rect(), QRegion::Ellipse));
        else
            clearMask();

        setWindowFlags(ctrl->windowFlags());
        enableBlurBehindExtended(this, ctrl);
        update();
    }
}

