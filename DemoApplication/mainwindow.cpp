#include "mainwindow.h"
#include "contentwidget.h"
#include "controlpanel.h"
#include "popuppanel.h"

#include "../ShapedWidget/shapedwidget.h"
#include "blurbehindeffect.h"

#include <QColorDialog>
#include <QImageReader>
#include <QStackedLayout>

#include <QImageWriter>
#include <QImageReader>

namespace
{
    constexpr QMargins kPanelLayoutMargins { 12, 12, 12, 12 };
    constexpr int kPanelLayoutSpacing = 8;
    constexpr std::chrono::milliseconds kMessageDuration(5000);
    constexpr int kMinimumHeight = 256 + kPanelLayoutMargins.top() + kPanelLayoutMargins.bottom();
    constexpr int kMaxPathTextWidth = 256;

    QVBoxLayout* createPanelLayout(QWidget* _parent)
    {
        QVBoxLayout* layout = new QVBoxLayout(_parent);
        layout->setContentsMargins(kPanelLayoutMargins);
        layout->setSpacing(kPanelLayoutSpacing);
        return layout;
    }

    static inline QString imagesFilter(const QList<QByteArray>& formats)
    {
        QString filter = QObject::tr("Images (");
        for (auto it = formats.begin(); it != formats.end(); )
        {
            filter += "*.";
            filter += *it;
            if (++it != formats.end())
                filter += ' ';
        }
        filter += ')';
        return filter;
    }

    static inline QString imagesLocation()
    {
        return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    }

    struct ApplicationCursorGuard
    {
        ApplicationCursorGuard(Qt::CursorShape _shape)
        {
            qApp->setOverrideCursor(_shape);
        }

        ~ApplicationCursorGuard()
        {
            qApp->restoreOverrideCursor();
        }
    };
}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    contentWidget_ = new ContentWidget(this);

    effect_ = new BlurBehindEffect(contentWidget_);
    effect_->setBlurMethod(BlurBehindEffect::BlurMethod::StackBlur);
    effect_->setDownsampleFactor(2.0);
    effect_->setBlurRadius(5);
    effect_->setBlurOpacity(0.7);
    effect_->setSourceOpacity(1.0);
    effect_->setBackgroundBrush(Qt::black);
    effect_->setCoordinateSystem(Qt::DeviceCoordinates);
    contentWidget_->setGraphicsEffect(effect_);

    QVBoxLayout* panelLayout = Q_NULLPTR;

    controlsLayer_ = new ShapedWidget(this);
    controlPanel_ = new ControlPanel(effect_, controlsLayer_);
    controlPanel_->onBrushColorChanged(contentWidget_->brushColor());
    controlPanel_->onPencilColorChanged(contentWidget_->pencilColor());
    controlPanel_->onClearColorChanged(contentWidget_->clearColor());
    connect(controlPanel_, &ControlPanel::paintToolRequested, contentWidget_, &ContentWidget::setCurrentTool);
    connect(controlPanel_, &ControlPanel::openImageRequested, this, &MainWindow::openImage);
    connect(controlPanel_, &ControlPanel::saveImageRequested, this, &MainWindow::saveImage);
    connect(controlPanel_, &ControlPanel::editPaintTool, this, &MainWindow::editPaintTool);
    connect(controlPanel_, &ControlPanel::clearColorRequested, this, &MainWindow::selectClearColor);
    connect(controlPanel_, &ControlPanel::clearContentsRequested, this, &MainWindow::clearImage);
    connect(contentWidget_, &ContentWidget::pencilColorChanged, controlPanel_, &ControlPanel::onPencilColorChanged);
    connect(contentWidget_, &ContentWidget::brushColorChanged, controlPanel_, &ControlPanel::onBrushColorChanged);
    connect(contentWidget_, &ContentWidget::clearColorChanged, controlPanel_, &ControlPanel::onClearColorChanged);

    panelLayout = createPanelLayout(controlsLayer_);
    panelLayout->addWidget(controlPanel_, 0, Qt::AlignBottom|Qt::AlignHCenter);

    messagesLayer_ = new ShapedWidget(this);
    messagePanel_ = new PopupPanel(PopupPanel::Category::MessagePanel, effect_, messagesLayer_);
    panelLayout = createPanelLayout(messagesLayer_);
    panelLayout->addWidget(messagePanel_, 0, Qt::AlignCenter);
    connect(messagePanel_, &PopupPanel::closeRequested, messagesLayer_, &ShapedWidget::hide);

    warningsLayer_ = new ShapedWidget(this);
    warningPanel_ = new PopupPanel(PopupPanel::Category::WarningPanel, effect_, warningsLayer_);
    panelLayout = createPanelLayout(warningsLayer_);
    panelLayout->addWidget(warningPanel_, 0, Qt::AlignTop|Qt::AlignHCenter);
    connect(warningPanel_, &PopupPanel::closeRequested, warningsLayer_, &ShapedWidget::hide);

    stackLayout_ = new QStackedLayout(this);
    stackLayout_->setStackingMode(QStackedLayout::StackAll);

    stackLayout_->addWidget(warningsLayer_);
    stackLayout_->addWidget(messagesLayer_);
    stackLayout_->addWidget(controlsLayer_);
    stackLayout_->addWidget(contentWidget_);

    messagesLayer_->hide();
    warningsLayer_->hide();

    setMinimumHeight(kMinimumHeight);
}

void MainWindow::openImage()
{
    warningsLayer_->hide();
    const QString filePath = QFileDialog::getOpenFileName(this, tr("Load Image"), imagesLocation(), imagesFilter(QImageWriter::supportedImageFormats()));
    if (filePath.isEmpty())
        return;

    ApplicationCursorGuard cursorGuard(Qt::WaitCursor);

    QImage image;
    QImageReader reader(filePath);
    if (!reader.read(&image))
    {
        Q_EMIT showWarning(tr("Failed to load image: %1").arg(reader.errorString()));
        return;
    }

    contentWidget_->setImage(image);
    const QString elidedPath = QFontMetrics(font()).elidedText(filePath, Qt::ElideMiddle, kMaxPathTextWidth);
    Q_EMIT showMessage(tr("Image '%1' was successfully loaded").arg(elidedPath));
}

void MainWindow::saveImage()
{
    warningsLayer_->hide();
    const QString filePath = QFileDialog::getSaveFileName(this, tr("Save Image"), imagesLocation(), imagesFilter(QImageWriter::supportedImageFormats()));
    if (filePath.isEmpty())
        return;

    ApplicationCursorGuard cursorGuard(Qt::WaitCursor);

    QImageWriter writer(filePath);
    if (!writer.write(contentWidget_->image()))
    {
        Q_EMIT showWarning(tr("Failed to save image: %1").arg(writer.errorString()));
        return;
    }

    const QString elidedPath = QFontMetrics(font()).elidedText(filePath, Qt::ElideMiddle, kMaxPathTextWidth);
    Q_EMIT showMessage(tr("Image was successfully saved into '%1'").arg(elidedPath));
}

void MainWindow::editPaintTool(PaintTool _tool)
{
    QDialog* dialog = contentWidget_->createEditDialog(_tool, this);
    if (!dialog)
        return;
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}

void MainWindow::selectClearColor()
{
    const QColor color = QColorDialog::getColor(contentWidget_->clearColor(), this, tr("Select Background Color"));
    if (color.isValid())
        contentWidget_->setClearColor(color);
}

void MainWindow::clearImage()
{
    warningsLayer_->hide();
    contentWidget_->clear();
}

void MainWindow::resizeEvent(QResizeEvent *_event)
{
    QWidget::resizeEvent(_event);
    effect_->setRegion(rect());
}

void MainWindow::showMessage(const QString& _text)
{
    messagePanel_->setText(_text);
    messagePanel_->setDuration(kMessageDuration);
    messagesLayer_->show();
}

void MainWindow::showWarning(const QString& _text)
{
    warningPanel_->setText(_text);
    warningsLayer_->show();
}




