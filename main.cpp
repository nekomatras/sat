#include <SFML/Graphics.hpp>
#include <thread>
#include "SeparatingAxisCollider.hpp"


int main()
{
    const std::string title = "Polygon in SFML";
    sf::RenderWindow window(sf::VideoMode({800, 600}), title);

    // Создаем выпуклый многоугольник
    auto trianglePtr = std::make_shared<Object>(1, std::initializer_list<Point>{{0.f, 0.f}, {0.f, 100.f}, {100.f, 100.f}});
    auto quadPtr = std::make_shared<Object>(2, std::initializer_list<Point>{{0.f, 100.f}, {100.f, 100.f}, {100.f, 0.f}, {0.f, 0.f}});
    auto quad2Ptr = std::make_shared<Object>(3, std::initializer_list<Point>{{100.f, 200.f}, {200.f, 200.f}, {200.f, 100.f}, {100.f, 100.f}});



    std::vector<std::shared_ptr<Object>> objects;
    objects.push_back(trianglePtr);
    objects.push_back(quadPtr);
    objects.push_back(quad2Ptr);

    //quad.rotate(sf::degrees(1)); //45/6.28

    float speed = 0.01f;
    SeparatingAxisCollider collider;
    collider.loadObjects({trianglePtr, quadPtr, quad2Ptr});
    sf::Font font{"/home/neko/.config/custom-fonts/visitor-tt2-brk/visitor1.ttf"};

    std::atomic<uint64_t> frames = 0;
    std::atomic<uint64_t> fps = 0;

    std::thread fpsMon([&]() {
        while (window.isOpen()) {
            fps = frames.load();
            frames = 0;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    while (window.isOpen())
    {
        frames++;
        window.clear();

        trianglePtr->setOutlineColor(Color::Green);
        quadPtr->setOutlineColor(Color::Green);
        quad2Ptr->setOutlineColor(Color::Green);

        while (const auto event = window.pollEvent()) 
        {
            if (event.value().is<sf::Event::Closed>())
                window.close();
        }

        // Двигаем полигон стрелками
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            trianglePtr->move({-speed, 0});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            trianglePtr->move({speed, 0});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            trianglePtr->move({0, -speed});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            trianglePtr->move({0, speed});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
            trianglePtr->rotate(sf::degrees(0.1));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
            trianglePtr->rotate(sf::degrees(-0.1));


        auto position = trianglePtr->getPosition();
        quadPtr->setPosition({500.f, 500.f});
        quad2Ptr->setPosition({100.f, 100.f});


        auto intersections = collider.getIntersections();
        for (const auto& [id, collisions] : intersections) {
            for (const auto& obj : objects) {
                if (obj->id == id && !collisions.empty()) {
                    obj->setOutlineColor(Color::Red);
                    for (const auto& collision : collisions) {
                        for (const auto& col : objects) {
                            if (col->id == collision) {
                                col->setOutlineColor(Color::Red);
                            }
                        }
                    }
                }
            }
        }


        auto resU = window.getSize();
        Vector res{static_cast<float>(resU.x), static_cast<float>(resU.y)};

        uint32_t textSize = 20;

        sf::Text resolution{font, res.toString(), textSize};
        resolution.setFillColor(sf::Color::Green);
        resolution.setPosition({0.f, 0.f});

        sf::Text fpsVal{font, std::to_string(fps), textSize};
        fpsVal.setFillColor(sf::Color::Green);
        fpsVal.setPosition({0.f, static_cast<float>(textSize) / 1.5f});

        //std::this_thread::sleep_for(std::chrono::milliseconds(1));

        window.draw(fpsVal);
        window.draw(resolution);

        window.draw(*trianglePtr);
        window.draw(Circle{position});

        window.draw(*quadPtr);
        window.draw(*quad2Ptr);
        window.display();
    }

    fpsMon.join();

    return 0;
}



