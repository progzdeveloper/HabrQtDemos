#pragma once
#include <QObject>
#include <QString>

using String = QString;

class Shape
{
    Q_GADGET

public:
    virtual ~Shape() = default;
    Shape() = default;
    Shape(const String& name)
        : m_name(name)
    {}

    virtual void draw(std::ostream& out) const = 0;

protected:
    String m_name;
};

class Circle : public Shape
{
    Q_GADGET
    Q_PROPERTY(int x MEMBER m_x)
    Q_PROPERTY(int y MEMBER m_y)
    Q_PROPERTY(double r MEMBER m_r)

public:
    Circle()
        : Circle("Circle", 0, 0, 0.0)
    {}
    Circle(int x, int y, double r)
        : Circle("Circle", x, y, r)
    {}
    Circle(const String& name, int x, int y, double r)
        : Shape(name)
        , m_x(x)
        , m_y(y)
        , m_r(r)
    {}

    void draw(std::ostream& out) const override
    {
        out << '\'' << m_name.toStdString() << '\''
            << " {" << m_x << ';' << m_y << "}: " << m_r;
    }

private:
    int m_x = 0, m_y = 0;
    double m_r = 0.0;
};



class Rectangle : public Shape
{
    Q_GADGET
    Q_PROPERTY(int x MEMBER m_x)
    Q_PROPERTY(int y MEMBER m_y)
    Q_PROPERTY(int w MEMBER m_w)
    Q_PROPERTY(int h MEMBER m_h)

public:
    Rectangle()
        : Rectangle("Rectangle", 0, 0, 0, 0)
    {}
    Rectangle(int x, int y, int w, int h)
        : Rectangle("Rectangle", x, y, w, h)
    {}
    Rectangle(const String& name, int x, int y, int w, int h)
        : Shape(name)
        , m_x(x)
        , m_y(y)
        , m_w(w)
        , m_h(h)
    {}

    void draw(std::ostream& out) const override
    {
        out << '\'' << m_name.toStdString() << '\''
            << " {" << m_x << ';' << m_y << "}: "
            << '[' << m_w << 'x' << m_h << ']';
    }

private:
    int m_x = 0, m_y = 0;
    int m_w = 0, m_h = 0;
};

class Line : public Shape
{
    Q_GADGET
    Q_PROPERTY(int x1 MEMBER m_x1)
    Q_PROPERTY(int y1 MEMBER m_y1)
    Q_PROPERTY(int x2 MEMBER m_x2)
    Q_PROPERTY(int y2 MEMBER m_y2)

public:
    Line()
        : Line("Line", 0, 0, 0, 0)
    {}
    Line(int x1, int y1, int x2, int y2)
        : Line("Line", x1, y1, x2, y2)
    {}
    Line(const String& name, int x1, int y1, int x2, int y2)
        : Shape(name)
        , m_x1(x1)
        , m_y1(y1)
        , m_x2(x2)
        , m_y2(y2)
    {}

    void draw(std::ostream& out) const override
    {
        out << '\'' << m_name.toStdString() << '\''
            << " {" << m_x1 << ';' << m_y1 << "}: "
            << '{' << m_x2 << ';' << m_y2 << '}';
    }

private:
    int m_x1 = 0, m_y1 = 0;
    int m_x2 = 0, m_y2 = 0;
};
