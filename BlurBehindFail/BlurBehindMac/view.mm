#include "widget.h"

#import <AppKit/AppKit.h>


Widget::Widget() : QWidget()
{
    winId();
    setAttribute(Qt::WA_TranslucentBackground);

    // native window dragging, smoother than handling mouseMoveEvent
    NSView *view = (NSView *)window()->effectiveWinId();
    NSWindow *wnd = [view window];
    [wnd setMovableByWindowBackground:YES];

    // setup the native vibrancy effect
    auto v = [[[NSVisualEffectView alloc] init] autorelease];
    v.material = NSVisualEffectMaterialHUDWindow;
    v.blendingMode = NSVisualEffectBlendingModeBehindWindow;
    v.state = NSVisualEffectStateActive;

    [wnd.contentView addSubview:v];
    [v setFrameSize:[ [ wnd contentView ] frame ].size];

    effectsView = v;

    QWidget* content = new QWidget(this);
    QPushButton* button = new QPushButton("Push Me!", content);
    QVBoxLayout* layout = new QVBoxLayout(content);
    layout->addWidget(button);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(content);

    NSView *contentView = (NSView *)content->winId();
    [wnd.contentView addSubview:contentView];
    content->show();
}


void Widget::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    NSView *view = (NSView *)window()->effectiveWinId();
    NSWindow *wnd = [view window];
    NSView* effect = (NSView*)effectsView;
    [effect setFrame:[ [ wnd contentView ] frame ]];
}

void Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    NSView *view = (NSView *)window()->effectiveWinId();
    NSWindow *wnd = [view window];
    NSView* effect = (NSView*)effectsView;
    [effect setFrame:[ [ wnd contentView ] frame ]];
}


