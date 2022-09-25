#ifndef SHARINGFRAME_H
#define SHARINGFRAME_H

#include <QFrame>

class QPushButton;
class SharingFrame : public QFrame
{
    Q_OBJECT

public:
    explicit SharingFrame(QScreen* _screen = Q_NULLPTR);

protected:
    void resizeEvent(QResizeEvent* _event) Q_DECL_OVERRIDE;

private:
    void updateMask(const QSize& _size);

private:
    QPushButton* closeButton_;
};

#endif // SHARINGFRAME_H
