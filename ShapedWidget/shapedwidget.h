#ifndef SHAPEDWIDGET_H
#define SHAPEDWIDGET_H

#include <QFrame>

class ShapedWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ShapedWidget(QWidget *_parent = Q_NULLPTR, Qt::WindowFlags _flags = Qt::WindowFlags{});

protected:
    virtual void updateMask();

    void resizeEvent(QResizeEvent* _event) Q_DECL_OVERRIDE;
    void childEvent(QChildEvent* _event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *_watched, QEvent *_event) Q_DECL_OVERRIDE;

private:
    QRegion regionMask() const;
    QRegion appendRegion(QWidget* _widget, const QRegion& _maskRegion);
    QRegion removeRegion(QWidget* _widget, const QRegion& _maskRegion);
};

#endif // SHAPEDWIDGET_H
