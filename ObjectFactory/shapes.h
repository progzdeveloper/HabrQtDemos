#pragma once
//#include <QObject> // comment out this line to test ObjectFactory without Qt support

#include <iostream>
#ifdef QT_VERSION
#include "shapes_qt.h"
#else
#include "shapes_cxx.h"
#endif
