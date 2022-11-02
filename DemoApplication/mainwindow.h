#pragma once
#include <QWidget>
#include "../MultiLayerWindow/painttool.h"

class ShapedWidget;

class ContentWidget;
class ControlPanel;
class PopupPanel;
class OverlayPanel;
class QStackedLayout;

class BlurBehindEffect;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* _parent = Q_NULLPTR);

    void openImage();
    void saveImage();
    void editPaintTool(PaintTool _tool);
    void selectClearColor();
    void clearImage();

protected:
    void resizeEvent(QResizeEvent* _event) Q_DECL_OVERRIDE;

private:
    void showMessage(const QString& _text);
    void showWarning(const QString& _text);

private:
    QString cachedSaveFolder_;
    QString cachedLoadFolder_;

    BlurBehindEffect* effect_;
    ContentWidget* contentWidget_;
    ControlPanel* controlPanel_;
    PopupPanel* warningPanel_;
    PopupPanel* messagePanel_;

    ShapedWidget* controlsLayer_;
    ShapedWidget* messagesLayer_;
    ShapedWidget* warningsLayer_;
    QStackedLayout* stackLayout_;
};
