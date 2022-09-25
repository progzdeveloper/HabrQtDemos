#ifndef MULTILAYERWINDOW_H
#define MULTILAYERWINDOW_H

#include <QWidget>
#include "painttool.h"

class ShapedWidget;

class ContentWidget;
class ControlPanel;
class PopupPanel;

class QStackedLayout;


class MultiLayerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MultiLayerWindow(QWidget *parent = Q_NULLPTR);

    void openImage();
    void saveImage();
    void editPaintTool(PaintTool _tool);
    void selectClearColor();
    void clearImage();

private:
    void showMessage(const QString& _text);
    void showWarning(const QString& _text);


private:
    ContentWidget* contentWidget_;
    ControlPanel* controlPanel_;
    PopupPanel* warningPanel_;
    PopupPanel* messagePanel_;

    ShapedWidget* controlsLayer_;
    ShapedWidget* messagesLayer_;
    ShapedWidget* warningsLayer_;
    QStackedLayout* stackLayout_;
};

#endif // MULTILAYERWINDOW_H
