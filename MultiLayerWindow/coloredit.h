#ifndef COLOREDIT_H
#define COLOREDIT_H

#include <QSpinBox>
#include <QLabel>

class ColorEdit : public QWidget
{
    Q_OBJECT
public:
    enum ColorChannel
    {
        Red = 0x1,
        Green = 0x2,
        Blue = 0x4,
        Alpha = 0x8
    };
    Q_DECLARE_FLAGS(Channels, ColorChannel)

    explicit ColorEdit(QWidget *parent = nullptr);

    void setChannels(Channels _channels);

public Q_SLOTS:
    void setColor(QColor _color);

Q_SIGNALS:
    void colorChanged(QColor);

private:
    void updateColor();
    void updateSpinBoxes();
    QSpinBox* createSpinBox();

private:
    QColor color_;
    Channels channels_;
    QSpinBox* rSpinBox;
    QSpinBox* gSpinBox;
    QSpinBox* bSpinBox;
    QSpinBox* aSpinBox;
    QLabel* labels[4];
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ColorEdit::Channels)

#endif // COLOREDIT_H
