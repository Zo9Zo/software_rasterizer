#include "image.hpp"
#include "config.hpp"

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <vector>

sf::Image NormalToGrayscale(const std::vector<float>& normal_values) {
    sf::Image image(config::window_size);
    for (uint32_t i = 0; i < config::window_size.y; ++i) {
        for (uint32_t j = 0; j < config::window_size.x; ++j) {
            float cur_value = normal_values[i * config::window_size.x + j];
            uint8_t intensity = static_cast<uint8_t>(cur_value * 255.0f);
            sf::Color color(intensity, intensity, intensity, 255);
            image.setPixel({j, i}, color);
        }
    }
    return image;
}