#ifndef WIGGLYWIDGET_H
#define WIGGLYWIDGET_H

#include <QtWidgets>

class WigglyWidget : public QWidget
{
    Q_OBJECT

public:
    WigglyWidget(QWidget *parent = 0);

public slots:
    void setText(const QString& newText) { text = newText; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void timerEvent(QTimerEvent* event) override;

private:
    QBasicTimer timer;
    QString text;
    int step;
};

#endif // WIGGLYWIDGET_H
