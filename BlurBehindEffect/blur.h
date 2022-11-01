#pragma once
#include <QImage>

QImage boxBlurImage(const QImage& _image, const QRect& _rect, int _radius);
inline QImage boxBlurImage(const QImage& _image, int _radius) { return boxBlurImage(_image, _image.rect(), _radius); }

QImage stackBlurImage(const QImage& _image, int _radius, int _threadCount = 1);


