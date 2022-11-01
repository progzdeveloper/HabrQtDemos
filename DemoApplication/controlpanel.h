#pragma once
#include "painttool.h"
#include "overlaypanel.h"
#include <QWidget>

class QMenu;
class CustomButton;

class ControlPanel : public OverlayPanel
{
    Q_OBJECT
public:
    explicit ControlPanel(BlurBehindEffect* _effect, QWidget *_parent = nullptr);

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
    void paintEvent(QPaintEvent* _event) Q_DECL_OVERRIDE;
    QPainterPath clipPath() const Q_DECL_OVERRIDE;

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
