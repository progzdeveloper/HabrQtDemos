#ifndef POPUPPANEL_H
#define POPUPPANEL_H

#include <chrono>
#include <QtWidgets>

class CustomButton;

class PopupPanel : public QWidget
{
    Q_OBJECT
public:
    enum class Category
    {
        WarningPanel,
        MessagePanel
    };

    explicit PopupPanel(Category _category, QWidget *_parent = nullptr);

    Category category() const;

    void setText(const QString& _text);
    QString text() const;

    void setDuration(std::chrono::milliseconds _duration);
    std::chrono::milliseconds duration() const;

Q_SIGNALS:
    void closeRequested();

protected:
    void showEvent(QShowEvent* _event);
    void paintEvent(QPaintEvent*);

private:
    QTimer* timer_;
    QLabel* label_;
    CustomButton* closeButton_;
    Category category_;
};

#endif // POPUPPANEL_H
