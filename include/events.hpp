#pragma once

#include <SFML/Graphics.hpp>

namespace EventHandler {
    struct Events {
        bool close_window = false;
        bool create_camera = false;
        int change_camera = -1;
        bool delete_camera = false;
        bool lock_camera = false;
    };

    Events HandleEvents(sf::RenderWindow& window);
}