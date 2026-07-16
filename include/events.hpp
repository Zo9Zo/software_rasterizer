#pragma once

#include <SFML/Graphics.hpp>

namespace EventHandler {
    struct Events {
        bool close_window = false;
    };

    Events HandleEvents(sf::RenderWindow& window);
}