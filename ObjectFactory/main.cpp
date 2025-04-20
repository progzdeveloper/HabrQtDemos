#include <cassert>
#include <array>
#include <memory>
#include <memory_resource>

#include "shapes.h"
#include "shapefactory.h"
#include "objectfactory.h"

struct destroyer
{
    template<class T>
    constexpr void operator()(T* arg) const {
        std::destroy_at(arg);
    }
};


template<class _Iterator>
void drawScene(_Iterator first, _Iterator last, std::ostream& out)
{
    for (; first != last; ++first)
    {
        const auto& shapePtr = *first;
        assert(shapePtr != nullptr);
        if (!shapePtr)
            continue;

        (*first)->draw(std::cout);
        std::cout << '\n';
    }
    std::cout << std::endl;
}

void dumpMetaInfo(const ObjectFactory<String, Shape>& factory)
{
#ifdef QT_VERSION
    const QStringList keys = factory.keys();
    for (const QString& k : keys)
        std::cout << "Creator of [" << factory.metaData(k)->className() << ']' << std::endl;
#endif
}

// Ex. 1: Using generic factory templated by ctor args
void genericFactory()
{
    using ShapePtr = std::unique_ptr<Shape>;

    // Create factory and registrate types
    GenericFactory<String, Shape> shapeFactory;
    shapeFactory.registrate<Rectangle>("Rect", []() { return new Rectangle{ "Rect", 0, 0, 10, 10 }; });
    shapeFactory.registrate<Circle>("Circle", []() { return new Circle{ "Circle", 0, 0, 2.5 }; });
    shapeFactory.registrate<Line>("Line", []() { return new Line{ "Line", 0, 0, 5, 5 }; });

    // Use factory to create shapes
    std::vector<ShapePtr> shapes;
    shapes.emplace_back(shapeFactory.create("Rect"));
    shapes.emplace_back(shapeFactory.create("Circle"));
    shapes.emplace_back(shapeFactory.create("Line"));

    std::cout << "Generic Factory" << std::endl;
    drawScene(shapes.cbegin(), shapes.cend(), std::cout);
}

// Ex. 2: Basic usage of object factory
void objectFactoryBasic()
{
    using ShapePtr = std::unique_ptr<Shape>;

    // Create factory and registrate types
    ObjectFactory<String, Shape> factory;
    factory.registrate<Line>("Line");
    factory.registrate<Line(String, int, int, int, int)>("Line");
    factory.registrate<Rectangle()>("Rect");
    factory.registrate<Rectangle(String, int, int, int, int)>("Rect");
    factory.registrate<Circle>("Circle");
    factory.registrate<Circle(String, int, int, double)>("Circle");

    // Use factory to create shapes
    std::vector<ShapePtr> shapes;
    shapes.emplace_back(factory.create<>("Rect"));
    shapes.emplace_back(factory.create<String, int, int, int, int>("Rect", "MyRect", 10, 10, 20, 20));
    shapes.emplace_back(factory.create<>("Circle"));
    shapes.emplace_back(factory.create<String, int, int, double>("Circle", "MyCircle", 5, 5, 0.5));
    shapes.emplace_back(factory.create<>("Line"));
    shapes.emplace_back(factory.create<String, int, int, int, int>("Line", "MyLine", 3, 3, 6, 6));

    std::cout << "Object Factory [basic]" << std::endl;
    dumpMetaInfo(factory);
    drawScene(shapes.cbegin(), shapes.cend(), std::cout);
}

// Ex. 3: Using object factory with custom allocator
void objectFactoryWithCustomAlloc()
{
    using ShapePtr = std::unique_ptr<Shape, destroyer>;

    // pool for Rectangle objects
    std::array<uint8_t, sizeof(Rectangle) * 4> rbuffer = { 0 };
    std::pmr::monotonic_buffer_resource mbr_rects{ rbuffer.data(), rbuffer.size() };
    std::pmr::polymorphic_allocator<uint8_t> rect_alloc{ &mbr_rects };

    // pool for Circle objects
    std::array<uint8_t, sizeof(Circle) * 4> cbuffer = { 0 };
    std::pmr::monotonic_buffer_resource mbr_circles{ cbuffer.data(), cbuffer.size() };
    std::pmr::polymorphic_allocator<uint8_t> circle_alloc{ &mbr_circles };

    // pool for Line objects
    std::array<uint8_t, sizeof(Line) * 4> lbuffer = { 0 };
    std::pmr::monotonic_buffer_resource mbr_lines{ lbuffer.data(), lbuffer.size() };
    std::pmr::polymorphic_allocator<uint8_t> line_alloc{ &mbr_lines };

    // Create factory and registrate types
    ObjectFactory<String, Shape> factory;
    factory.registrate<Line>("Line");
    factory.registrate<Line(String, int, int, int, int)>("Line");
    factory.registrate<Rectangle()>("Rect");
    factory.registrate<Rectangle(String, int, int, int, int)>("Rect");
    factory.registrate<Circle>("Circle");
    factory.registrate<Circle(String, int, int, double)>("Circle");

    // Use factory to create shapes
    std::vector<ShapePtr> shapes;
    shapes.emplace_back(factory.construct<decltype(rect_alloc)>(rect_alloc, "Rect"));
    shapes.emplace_back(factory.construct<decltype(rect_alloc), String, int, int, int, int>(rect_alloc, "Rect", "MyRect", 10, 10, 20, 20));
    shapes.emplace_back(factory.construct<decltype(circle_alloc)>(circle_alloc, "Circle"));
    shapes.emplace_back(factory.construct<decltype(circle_alloc), String, int, int, double>(circle_alloc, "Circle", "MyCircle", 5, 5, 0.5));
    shapes.emplace_back(factory.construct<decltype(line_alloc)>(line_alloc, "Line"));
    shapes.emplace_back(factory.construct<decltype(line_alloc), String, int, int, int, int>(line_alloc, "Line", "MyLine", 3, 3, 6, 6));

    std::cout << "Object Factory [custom alloc]" << std::endl;
    dumpMetaInfo(factory);
    drawScene(shapes.cbegin(), shapes.cend(), std::cout);
}

// Ex. 4: Using the singletone auto-registering factory
void singletoneObjectFactory()
{
    using ShapePtr = std::unique_ptr<Shape, destroyer>;

    // Use global factory to create shapes
    std::vector<ShapePtr> shapes;
    shapes.emplace_back(shapeFactory().create("Rectangle"));
    shapes.emplace_back(shapeFactory().create("Line"));
    shapes.emplace_back(shapeFactory().create("Circle"));
    shapes.emplace_back(shapeFactory().create<String, int, int, double>("Circle", "MyCircle", 1, 1, 10.5));

    std::cout << "Singletone Object Factory" << std::endl;
    dumpMetaInfo(shapeFactory());
    drawScene(shapes.cbegin(), shapes.cend(), std::cout);
}


int main()
{
    genericFactory();
    objectFactoryBasic();
    objectFactoryWithCustomAlloc();
    singletoneObjectFactory();
    return 0;
}
