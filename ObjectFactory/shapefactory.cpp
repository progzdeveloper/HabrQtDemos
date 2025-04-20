#include "shapefactory.h"

REGISTRATE_SHAPE_CLASS(Circle, 0)
REGISTRATE_SHAPE_CLASS(Circle, 1, String, int, int, double)
REGISTRATE_SHAPE_CLASS(Rectangle, 0)
REGISTRATE_SHAPE_CLASS(Line, 0)

ShapeFactory &shapeFactory()
{
    static ShapeFactory instance;
    return instance;
}
