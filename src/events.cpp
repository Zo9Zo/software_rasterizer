#include "events.hpp"

#include <SFML/Graphics.hpp>
#include <optional>

EventHandler::Events EventHandler::HandleEvents(sf::RenderWindow& window) {
    EventHandler::Events events;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            events.close_window = true;
        }
        else if (const auto* key_pressed_event = event->getIf<sf::Event::KeyPressed>()) {
            if (key_pressed_event->code == sf::Keyboard::Key::F) {
                events.create_camera = true;
            }
            if (key_pressed_event->code == sf::Keyboard::Key::Escape) {
                events.close_window = true;
            }
            if (static_cast<int>(key_pressed_event->code) >= 26 &&
                static_cast<int>(key_pressed_event->code) <= 35) {
                int code_index = static_cast<int>(key_pressed_event->code) - 26;
                if (code_index == 0) {
                    events.change_camera = 9;
                } else {
                    events.change_camera = code_index - 1;
                }
            }
            if (key_pressed_event->code == sf::Keyboard::Key::L) {
                events.lock_camera = true;
            }
            if (key_pressed_event->code == sf::Keyboard::Key::G) {
                events.delete_camera = true;
            }
        }
    }
    return events;
}