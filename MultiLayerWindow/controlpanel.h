#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include "painttool.h"
#include <QWidget>

class QMenu;
class CustomButton;

class ControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ControlPanel(QWidget *parent = nullptr);

public Q_SLOTS:
    void onPencilColorChanged(QColor c);
    void onBrushColorChanged(QColor c);
    void onClearColorChanged(QColor c);

Q_SIGNALS:
    void openImageRequested();
    void saveImageRequested();

    void paintToolRequested(PaintTool);
    void editPaintTool(PaintTool);
    void clearContentsRequested();
    void clearColorRequested();

    void settingsRequested();
    void toggleFullScreen();

protected:
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;

private:
    QMenu* createFileMenu();
    QMenu* createSettingsMenu();
private:
    QMenu* fileMenu_;
    QMenu* settingsMenu_;
    CustomButton* fileMenuButton_;
    CustomButton* pencilButton_;
    CustomButton* brushButton_;
    CustomButton* eraserButton_;
    CustomButton* settingsButton_;
    CustomButton* clearButton_;
};

#endif // CONTROLPANEL_H
