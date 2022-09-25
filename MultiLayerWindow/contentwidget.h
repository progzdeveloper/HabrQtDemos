#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QWidget>
#include <QColor>
#include <QImage>
#include <QPoint>

#include "painttool.h"

class ContentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ContentWidget(QWidget *_parent = Q_NULLPTR);

    PaintTool currentTool() const;

    void setPencilColor(QColor _color);
    QColor pencilColor() const;

    void setBrushColor(QColor _color);
    QColor brushColor() const;

    void setClearColor(QColor _color);
    QColor clearColor() const;

    void setPencilWidth(int _w);
    int pencilWidth() const;

    void setBrushWidth(int _w);
    int brushWidth() const;

    void setEraserWidth(int _w);
    int eraserWidth() const;

    void setImage(const QImage& _image);
    const QImage& image() const;


    QDialog* createEditDialog(PaintTool _tool, QWidget *_parent = Q_NULLPTR);


public Q_SLOTS:
    void setCurrentTool(PaintTool _tool);
    void clear();

Q_SIGNALS:
    void pencilColorChanged(QColor);
    void brushColorChanged(QColor);
    void clearColorChanged(QColor);

protected:
    void paintEvent(QPaintEvent* _event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* _event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* _event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* _event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* _event) Q_DECL_OVERRIDE;

private:
    void drawLineTo(const QPoint &_endPoint);
    void eraserLineTo(const QPoint& _endPoint);
    void pencilLineTo(const QPoint &_endPoint);
    void brushLineTo(const QPoint& _endPoint);
    void updateCursor();

    void resizeImage(QImage *_image, const QSize &_newSize);

private:
    QImage image_;
    QPoint lastPoint_;
    QColor penColor_;
    QColor brushColor_;
    QColor clearColor_;
    QColor currentClearColor_;
    PaintTool tool_;
    int penWidth_;
    int brushWidth_;
    int eraserWidth_;
};

#endif // CONTENTWIDGET_H
