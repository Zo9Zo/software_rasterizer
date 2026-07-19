#pragma once

#include <SFML/Graphics.hpp>
#include <array>

namespace config {
    inline constexpr sf::Vector2u window_size = {800, 800};
    inline constexpr std::array<float, 3> initial_camera_forward = {0, 1, 0};
    inline constexpr std::array<float, 3> initial_camera_position = {0, 0, 0};
    inline constexpr float near_plane_distance = 0.1f;
    inline constexpr float far_plane_distance = 100.0f;
    inline constexpr float aspect_ratio = static_cast<float>(config::window_size.x) / 
                                          static_cast<float>(config::window_size.y);
    inline constexpr sf::Angle fov = sf::degrees(60.0f);
    inline constexpr float movement_speed = 10.0f;
};