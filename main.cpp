#include <SFML/Graphics.hpp>
#include <initializer_list>

typedef sf::Vector2f Point;
typedef sf::CircleShape Circle;
typedef sf::Color Color;

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

    quad.rotate(sf::degrees(45)); //45/6.28

    float speed = 0.01f;

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


            
        auto position = polygon.getPosition();
        quad.setPosition({500.f, 500.f});

        window.clear();
        window.draw(polygon);
        window.draw(makeCircle(position));

        window.draw(quad);
        window.display();
    }

    return 0;
}