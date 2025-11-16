#include <SFML/Graphics.hpp>
#include <algorithm>
#include <initializer_list>
#include <cmath>
#include <iostream>

typedef sf::Vector2f Point;
typedef sf::CircleShape Circle;
typedef sf::Color Color;

std::string toString(Point point) {
    return "[" + std::to_string(point.x) + ", " + std::to_string(point.y) + "]";
};

struct VertexLine : sf::VertexArray 
{
    VertexLine() : sf::VertexArray(sf::PrimitiveType::Lines, 2) {}

    VertexLine(Point startPoint, Color startColor, Point endPoint, Color endColor) : sf::VertexArray(sf::PrimitiveType::Lines, 2) 
    {
        setStartPoint(startPoint);
        setStartColor(startColor);
        setEndPoint(endPoint);
        setEndColor(endColor);
    }

    Point getStartPoint() const
    {
        return (*this)[0].position;
    }

    Point getEndPoint() const
    {
        return (*this)[1].position;
    }

    void setStartPoint(Point point)
    {
        (*this)[0].position = point;
    }

    void setEndPoint(Point point)
    {
        (*this)[1].position = point;
    }

    Color getStartColor() const
    {
        return (*this)[0].color;
    }

    Color getEndColor() const
    {
        return (*this)[1].color;
    }

    void setStartColor(Color color)
    {
        (*this)[0].color = color;
    }

    void setEndColor(Color color)
    {
        (*this)[1].color = color;
    }
};

VertexLine makeVertexLine(Point start, Point end, float thickness = 1)
{
    return {start, Color::Red, end, Color::Blue};
}

struct Line
{
    Point start = {0.f, 0.f};
    Point end = {0.f, 0.f};
    float thickness = 1;

    Line(Point start, Point end) : start(start), end(end) {};

    VertexLine makeVertexLine() const
    {
        return {start, Color::Red, end, Color::Blue};
    }
};

struct Polygon : sf::ConvexShape
{
    Point getRealPoint(std::size_t index) const
    {
        Point imgPoint = sf::ConvexShape::getPoint(index);
        return sf::ConvexShape::getTransform().transformPoint(imgPoint);
    }

    Point getCenter() const
    {
        // Получаем список вершин многоугольника
        int pointCount = sf::ConvexShape::getPointCount();
        float sumX = 0.f, sumY = 0.f;

        // Суммируем координаты всех вершин
        for (int i = 0; i < pointCount; ++i) {
            Point point = sf::ConvexShape::getPoint(i);
            sumX += point.x;
            sumY += point.y;
        }

        // Находим среднее значение для X и Y
        return Point(sumX / pointCount, sumY / pointCount);
    }
};

enum Axis
{
    X,
    Y,
};




Circle makeCircle(Point center)
{
    // Создаем круг
    Circle circle(1); // Радиус 50 пикселей
    circle.setFillColor(sf::Color::Blue); // Цвет заливки
    circle.setOutlineThickness(5); // Толщина обводки
    circle.setOutlineColor(sf::Color::Black); // Цвет обводки
    circle.setPosition({center.x, center.y}); // Устанавливаем позицию круга
    return circle;
}

Polygon makePolygon(std::initializer_list<Point> points)
{
    auto pointsQty = points.size();
    Polygon polygon;
    polygon.setPointCount(pointsQty);

    int pointNumber = 0;
    for (const auto& point : points)
    {
        polygon.setPoint(pointNumber, point);
        pointNumber++;
    }

    if (pointNumber != pointsQty)
    {
        throw new std::logic_error("Wrong points number");
    }
    
    polygon.setFillColor(Color(0, 0, 0, 0));
    polygon.setOutlineColor(Color::Green);
    polygon.setOutlineThickness(2.f);
    polygon.setOrigin(polygon.getCenter());
    return polygon;
}

/*
Separating Axis Theorem для двух выпуклых объектов можно сформулировать так:
два выпуклых объекта пересекаются тогда и только тогда, когда существует плоскость
(для двумерного случая - прямая), такая, что одна геометрия лежит по одну ей сторону, 
а другая - по другую.
*/
class Collider {

};

class SeparatingAxisCollider : Collider {

    sf::RenderWindow& window;

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

            std::string toString() const {
                return  std::to_string(a) + " * x + " + std::to_string(b) + " * y + " + std::to_string(c) + " = 0";
            }
        };

        Point start{0, 0};
        Point end{0, 0};
        Line(){};
        Line(Point start, Point end) : start(start), end(end){};

        sf::Vector2f getVector() const {
            LineCoefficients coefficients = getCoefficients();
            return {coefficients.b, -coefficients.a};
        }

        /* Point getNormalVector() const {
            auto vector = getVector();
            return {vector.y, -vector.x};
        } */

        // Получаем коэффициенты уровнения прямой
        LineCoefficients getCoefficients() const {

            //std::cout << toString(start) << " - " << toString(end) << std::endl;
            // (x / (xb - xa)) - (xa / (xb - xa)) = ((y / (yb - ya)) - (ya / (yb - ya)))
            // (x / (xb - xa)) - ((y / (yb - ya)) = - (ya / (yb - ya))) + (xa / (xb - xa))

            float a = 1 / (end.x - start.x);
            float b = -1 / (end.y - start.y);
            float c = - (start.y / (end.y - start.y)) + (start.x / (end.x - start.x));

            LineCoefficients coef{a, b, c};
            
            //std::cout << "Orig: " << coef.toString() << std::endl;
            return coef;

            //Point normalVector = getNormalVector();
            //return getCoefficients(normalVector);
        }

        // Получаем коэффициенты уровнения нормали к прямой
        LineCoefficients getNormalCoefficients() const {
            LineCoefficients line = getCoefficients();
            return {line.b, -line.a, line.a * start.y - line.b * start.x};

            /* auto vector = getVector();
            return getCoefficients(vector); */
        }


    private:
        /* LineCoefficients getCoefficients(const Point& vector) const {
            auto a = vector.x;
            auto b = vector.y;
            auto c = -(a * start.x + b * start.y);
            return {a, b, c};
        } */
    };

    // Получаем ребра многоугольника
    std::vector<Line> getPolygonSides(const Polygon& polygon) const {
        auto numberOfPoints = polygon.getPointCount();

        std::vector<Line> lines;
        lines.reserve(numberOfPoints);

        auto getLineFormPoints = [&](std::size_t startIndex, std::size_t endIndex) {
            auto start = polygon.getRealPoint(startIndex);
            auto end = polygon.getRealPoint(endIndex);
            lines.emplace_back(start, end);
        };

        for (std::size_t i = 0; i < numberOfPoints - 1; ++i) {
            getLineFormPoints(i, i + 1);
        }

        getLineFormPoints(numberOfPoints - 1, 0);

        return lines;
    }

    std::vector<Line::LineCoefficients> getPolygonSidesNormalCoefficients(const Polygon& polygon) const {
        auto numberOfPoints = polygon.getPointCount();

        if (numberOfPoints < 3) {
            throw std::logic_error(
                "Unexpected number of points in polyugon: "
                + std::to_string(numberOfPoints)
            );
        }

        auto lines = getPolygonSides(polygon);

        std::vector<Line::LineCoefficients> axises;
        axises.reserve(numberOfPoints);

        for (const Line line : lines) {
            axises.push_back(line.getNormalCoefficients());
        }

        return axises;
    }

    bool isLineIntersected(sf::Vector2f a, sf::Vector2f b) const {

        if (b.y > a.x && b.x < a.y) {
            return true;
        }

        return false;
    }

    sf::Vector2f getPolygonProjection(const Polygon& polygon, const Line::LineCoefficients axis) const {

        auto numberOfPoints = polygon.getPointCount();

        std::vector<float> projections;
        projections.reserve(numberOfPoints);

        for (std::size_t i = 0; i < numberOfPoints - 1; ++i) {
            auto point = polygon.getRealPoint(i);
            auto pointProjection = getPointProjection(point, axis);
            projections.push_back(pointProjection);
        }

        auto x = *std::ranges::min_element(projections);
        auto y = *std::ranges::max_element(projections);

        return {x, y};
    }

    /* Line getLineProjection(const Line& line, const Line::LineCoefficients axis) const {
        return {getPointProjection(line.start, axis), getPointProjection(line.end, axis)};
    } */

    /* Point getPointProjection(const Point& point, const Line::LineCoefficients axis) const {

        auto xProjection = point.x - (axis.a * ((axis.a * point.x + axis.b * point.y + axis.c) / (axis.a * axis.a + axis.b * axis.b)));
        auto yProjection = point.y - (axis.b * ((axis.a * point.x + axis.b * point.y + axis.c) / (axis.a * axis.a + axis.b * axis.b)));

        return {xProjection, yProjection};
    } */

    float getPointProjection(const Point& point, const Line::LineCoefficients axis) const {
        sf::Vector2f vector{-axis.b, axis.a};
        auto vectorAbs = std::sqrt(vector.x * vector.x + vector.y * vector.y);
        return (point.x * vector.x + point.y * vector.y) / vectorAbs;
    }

public:
    bool isIntersect(const Polygon& a, const Polygon& b) {

        auto aAxis = getPolygonSidesNormalCoefficients(a);
        auto bAxis = getPolygonSidesNormalCoefficients(b);

        std::vector<Line::LineCoefficients> axisToCheck;
        axisToCheck.reserve(aAxis.size() + bAxis.size());

        std::copy(aAxis.cbegin(), aAxis.cend(), std::back_inserter(axisToCheck));
        std::copy(bAxis.cbegin(), bAxis.cend(), std::back_inserter(axisToCheck));

        for (auto axis : axisToCheck) {

            /* std::cout << axis.toString() << std::endl;
            // axis.a * x + axis.b * y + axis.c;
            float y0 = 0;
            auto x0 = (- axis.c - (axis.b * y0)) / axis.a;
            float y1 = 1000;
            auto x1 = (- axis.c - (axis.b * y1)) / axis.a;
            auto ax = VertexLine{{x0, y0}, Color::Magenta, {x1, y1}, Color::Magenta};
    
            //std::cout << "[" << x0 << ", " << y0 << "] - [" << x1 << ", " << y1 << "]" << std::endl;

            window.draw(ax); */

            auto aProjection = getPolygonProjection(a, axis);
            auto bProjection = getPolygonProjection(b, axis);

            //std::cout << "[" << aProjection.x << ", " << aProjection.y << "] - [" << bProjection.x << ", " << bProjection.y << "]" << std::endl;

            if (!isLineIntersected(aProjection, bProjection)) {
                return false;
            }
        }

        return true;
    }

    SeparatingAxisCollider(sf::RenderWindow& window) : window(window) {};
};

int main()
{
    const std::string title = "Polygon in SFML";
    sf::RenderWindow window(sf::VideoMode({800, 600}), title);

    // Создаем выпуклый многоугольник
    Polygon polygon = makePolygon(
        {
            {0.f, 0.f}, 
            {0.f, 100.f}, 
            {100.f, 100.f}, 
            {100.f, 0.f}, 
        });

    Polygon quad = makePolygon(
        {
            {0.f, 0.f}, 
            {0.f, 100.f}, 
            {100.f, 100.f}, 
            {100.f, 0.f}, 
        });

    quad.rotate(sf::degrees(30)); //45/6.28

    float speed = 0.01f;
    SeparatingAxisCollider collider{window};

    while (window.isOpen())
    {
        polygon.setOutlineColor(Color::Green);
        quad.setOutlineColor(Color::Green);

        //window.pollEvent();
        //sf::Event event;
        while (const auto event = window.pollEvent()) 
        {
            if (event.value().is<sf::Event::Closed>())
                window.close();
        }

        // Двигаем полигон стрелками
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            polygon.move({-speed, 0});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            polygon.move({speed, 0});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            polygon.move({0, -speed});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            polygon.move({0, speed});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
            polygon.rotate(sf::degrees(0.1));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
            polygon.rotate(sf::degrees(-0.1));

        window.clear();


        auto position = polygon.getPosition();
        quad.setPosition({500.f, 500.f});

        if (collider.isIntersect(polygon, quad)) {
            polygon.setOutlineColor(Color::Red);
            quad.setOutlineColor(Color::Red);
        }

        window.draw(polygon);
        window.draw(makeCircle(position));

        window.draw(quad);
        window.display();
    }

    return 0;
}



