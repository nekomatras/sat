#pragma once

#include "Renderer.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>



using Drawable = sf::Drawable;

class SfmlRenderer : Renderer {

    std::shared_ptr<sf::RenderWindow> window;

    uint8_t targetFrameRate = 60;

    std::shared_ptr<std::vector<std::shared_ptr<Drawable>>> objectsToDraw;

public:

    void drawObjects() {
        window->clear();

        if (objectsToDraw == nullptr) {
            return;
        }

        for (const auto& object : *objectsToDraw) {
            if (object != nullptr) {
                window->draw(*object);
            }
        }

        window->display();
    }

public:

    SfmlRenderer(std::shared_ptr<std::vector<std::shared_ptr<Drawable>>> objects, std::shared_ptr<sf::RenderWindow> window) 
        : objectsToDraw(objects), window(window) {}

    void run() {
        while (window->isOpen()) {
            drawObjects();
        }
    }
};