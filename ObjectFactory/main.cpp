//#include <QObject> // comment out this line to test ObjectFactory without Qt support

#include "objectfactory.h"
#include "shapes.h"
#include "shapefactory.h"

struct destroyer
{
    template<class T>
    constexpr void operator()(T* arg) const {
        std::destroy_at(arg);
    }
};

int main()
{
    using ShapePtr = std::unique_ptr<Shape>;

    std::vector<ShapePtr> shapes;
    shapes.emplace_back(shapeFactory().create("Circle"));
    shapes.emplace_back(shapeFactory().create<String, int, int, double>("Circle", "MyCircle", 1, 1, 10.5));
    shapes.emplace_back(shapeFactory().create("Rectangle"));
    return 0;
}
