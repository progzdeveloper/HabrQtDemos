#include "controlpanel.h"
#include "../CustomButton/custombutton.h"
#include <algorithm>

#include <QPainter>
#include <QtWidgets>
#include <QApplication>

namespace
{
    constexpr QMargins kLayoutMargins{16, 12, 16, 12};
    constexpr int kLayoutSpacing = 24;
    constexpr qreal kPanelRadius = 12;
    constexpr qreal kPanelOpacity = 0.7;
}

ControlPanel::ControlPanel(QWidget *parent) : QWidget(parent)
{
    fileMenuButton_ = new CustomButton(tr("File"), this);
    fileMenuButton_->setMenu(createFileMenu());
    fileMenuButton_->setIcon(QIcon(":/icons/image"));
    fileMenuButton_->setCursor(Qt::PointingHandCursor);
    fileMenuButton_->setPopupMode(CustomButton::InstantPopup);

    pencilButton_ = new CustomButton(tr("Pencil"), this);
    pencilButton_->setIcon(QIcon(":/icons/pencil"));
    pencilButton_->setCursor(Qt::PointingHandCursor);
    pencilButton_->setCheckable(true);
    pencilButton_->setChecked(true);
    pencilButton_->setAutoExclusive(true);
    connect(pencilButton_, &CustomButton::clicked, this, [this]() { Q_EMIT paintToolRequested(PaintTool::Pencil); });
    connect(pencilButton_, &CustomButton::menuRequested, this, [this]() { Q_EMIT editPaintTool(PaintTool::Pencil); });

    brushButton_ = new CustomButton(tr("Brush"), this);
    brushButton_->setIcon(QIcon(":/icons/brush"));
    brushButton_->setCursor(Qt::PointingHandCursor);
    brushButton_->setCheckable(true);
    brushButton_->setAutoExclusive(true);
    connect(brushButton_, &CustomButton::clicked, this, [this]() { Q_EMIT paintToolRequested(PaintTool::Brush); });
    connect(brushButton_, &CustomButton::menuRequested, this, [this]() { Q_EMIT editPaintTool(PaintTool::Brush); });

    eraserButton_ = new CustomButton(tr("Eraser"), this);
    eraserButton_->setIcon(QIcon(":/icons/eraser"));
    eraserButton_->setCursor(Qt::PointingHandCursor);
    eraserButton_->setCheckable(true);
    eraserButton_->setAutoExclusive(true);
    connect(eraserButton_, &CustomButton::clicked, this, [this]() { Q_EMIT paintToolRequested(PaintTool::Eraser); });
    connect(eraserButton_, &CustomButton::menuRequested,  this, [this]() { Q_EMIT editPaintTool(PaintTool::Eraser); });

    clearButton_ = new CustomButton(tr("Clear"), this);
    clearButton_->setIcon(QIcon(":/icons/trash"));
    clearButton_->setCursor(Qt::PointingHandCursor);
    connect(clearButton_, &CustomButton::clicked, this, &ControlPanel::clearContentsRequested);
    connect(clearButton_, &CustomButton::menuRequested, this, &ControlPanel::clearColorRequested);

    settingsButton_ = new CustomButton(tr("More"), this);
    settingsButton_->setIcon(QIcon(":/icons/more-horizontal"));
    settingsButton_->setCursor(Qt::PointingHandCursor);
    settingsButton_->setPopupMode(CustomButton::InstantPopup);
    settingsButton_->setMenu(createSettingsMenu());

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(kLayoutMargins);
    layout->setSpacing(kLayoutSpacing);
    layout->addWidget(fileMenuButton_);
    layout->addWidget(pencilButton_);
    layout->addWidget(brushButton_);
    layout->addWidget(eraserButton_);
    layout->addWidget(settingsButton_);
    layout->addWidget(clearButton_);
}

void ControlPanel::onPencilColorChanged(QColor c)
{
    pencilButton_->setBadgeValue(c);
}

void ControlPanel::onBrushColorChanged(QColor c)
{
    c.setAlpha(255);
    brushButton_->setBadgeValue(c);
}

void ControlPanel::onClearColorChanged(QColor c)
{
    clearButton_->setBadgeValue(c);
}

void ControlPanel::paintEvent(QPaintEvent*)
{
    const QRect r = rect().adjusted(0, 0, -1, -1);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setOpacity(kPanelOpacity);
    painter.drawRoundedRect(r, kPanelRadius, kPanelRadius);
}

QMenu *ControlPanel::createFileMenu()
{
    QMenu* menu = new QMenu(this);
    menu->addAction(tr("New Image..."), this, &ControlPanel::clearContentsRequested);
    menu->addAction(QIcon(":/icons/open"), tr("Load Image..."), this, &ControlPanel::openImageRequested);
    menu->addAction(QIcon(":/icons/save"), tr("Save Image..."), this, &ControlPanel::saveImageRequested);
    menu->addAction(tr("Exit"), qApp, &QApplication::quit);
    return menu;
}

QMenu *ControlPanel::createSettingsMenu()
{
    QMenu* menu = new QMenu(this);
    menu->addAction(tr("Full Screen"), this, &ControlPanel::toggleFullScreen);
    menu->addAction(tr("Preferences..."));
    return menu;
}
