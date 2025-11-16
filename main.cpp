#include <SFML/Graphics.hpp>
#include <thread>
#include "SeparatingAxisCollider.hpp"


int main()
{
    const std::string title = "Polygon in SFML";
    sf::RenderWindow window(sf::VideoMode({800, 600}), title);

    // Создаем выпуклый многоугольник
    Polygon polygon{
            {0.f, 0.f},
            {0.f, 100.f},
            {100.f, 100.f},
            //{100.f, 0.f},
        };

    Polygon quad{
            //{0.f, 0.f},
            {0.f, 100.f},
            {100.f, 100.f},
            {100.f, 0.f},
        };

    //quad.rotate(sf::degrees(1)); //45/6.28

    float speed = 0.01f;
    SeparatingAxisCollider collider;
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

        polygon.setOutlineColor(Color::Green);
        quad.setOutlineColor(Color::Green);

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

        if (collider.isIntersect(polygon, quad)) {
            polygon.setOutlineColor(Color::Red);
            quad.setOutlineColor(Color::Red);
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

        window.draw(polygon);
        window.draw(Circle{position});

        window.draw(quad);
        window.display();
    }

    fpsMon.join();

    return 0;
}



