#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <initializer_list>



using Color = sf::Color;

inline const float EPS = 0.0000000000000001f;

inline float subtractFloat(float a, float b) {
    float result = a - b;
    return (std::abs(result) < EPS) ? EPS : result;
}

struct Point : sf::Vector2f {

    using sf::Vector2f::Vector2f;

    Point(const sf::Vector2f& v) : sf::Vector2f(v) {}

    std::string toString() {
        return "[" + std::to_string(x) + ", " + std::to_string(y) + "]";
    };

};

using Vector = Point;

enum Axis {
    X,
    Y,
};

struct Polygon : sf::ConvexShape {

    Polygon(std::initializer_list<Point> points) {
        auto pointsQty = points.size();
        setPointCount(pointsQty);

        int pointNumber = 0;
        for (const auto& point : points) {
            setPoint(pointNumber, point);
            pointNumber++;
        }

        if (pointNumber != pointsQty) {
            throw new std::logic_error("Wrong points number");
        }

        setFillColor(Color(0, 0, 0, 0));
        setOutlineColor(Color::Green);
        setOutlineThickness(2.f);
        setOrigin(getCenter());
    }

    Point getRealPoint(std::size_t index) const {
        auto imgPoint = sf::ConvexShape::getPoint(index);
        return sf::ConvexShape::getTransform().transformPoint(imgPoint);
    }

    Point getCenter() const {
        // Получаем список вершин многоугольника
        int pointCount = sf::ConvexShape::getPointCount();
        float sumX = 0.f, sumY = 0.f;

        // Суммируем координаты всех вершин
        for (int i = 0; i < pointCount; ++i) {
            auto point = sf::ConvexShape::getPoint(i);
            sumX += point.x;
            sumY += point.y;
        }

        // Находим среднее значение для X и Y
        return Point(sumX / pointCount, sumY / pointCount);
    }
};


struct Object : Polygon{
    using TId = uint64_t;

    TId id;

    Object(uint64_t id, std::initializer_list<Point> points)
        : id(id), Polygon(points) {}

    bool operator< (Object other) {
        return id < other.id;
    };
};

class Line {
public:

    struct LineCoefficients {
        float a{0};
        float b{0};
        float c{0};

        LineCoefficients(){};
        LineCoefficients(float a, float b, float c)
            : a{a}
            , b{b}
            , c{c}
        {};

        LineCoefficients getNormal(Point pointOnLine) const {
            return {b, -a, a * pointOnLine.y - b * pointOnLine.x};
        }

        std::string toString() const {
            return  std::to_string(a) + " * x + " + std::to_string(b) + " * y + " + std::to_string(c) + " = 0";
        }
    };

    Point start{0, 0};
    Point end{0, 0};
    Line(){};
    Line(Point start, Point end) : start(start), end(end){};

    Vector getVector() const {
        LineCoefficients coefficients = getCoefficients();
        return {coefficients.b, -coefficients.a};
    }

    // Получаем коэффициенты уровнения прямой
    LineCoefficients getCoefficients() const {
        // (x / (xb - xa)) - (xa / (xb - xa)) = ((y / (yb - ya)) - (ya / (yb - ya)))
        // (x / (xb - xa)) - ((y / (yb - ya)) = - (ya / (yb - ya))) + (xa / (xb - xa))

        float a = 1 / subtractFloat(end.x, start.x);
        float b = -1 / subtractFloat(end.y, start.y);
        float c = - (start.y / subtractFloat(end.y, start.y)) + (start.x / subtractFloat(end.x, start.x));

        LineCoefficients coef{a, b, c};
        return coef;
    }

    // Получаем коэффициенты уровнения нормали к прямой
    LineCoefficients getNormalCoefficients() const {
        LineCoefficients line = getCoefficients();
        Point mid{(start.x + end.x) / 2, (start.y + end.y) / 2};
        return line.getNormal(mid);
    }
};

struct Circle : sf::CircleShape {
    Circle(Point center) : sf::CircleShape(1) {
        setFillColor(sf::Color::Blue);
        setOutlineThickness(5);
        setOutlineColor(sf::Color::Black);
        setPosition({center.x, center.y});
    }
};