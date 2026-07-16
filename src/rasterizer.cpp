#include "rasterizer.hpp"
#include "config.hpp"
#include "geometry.hpp"
#include "transform.hpp"
#include "math.hpp"

#include <array>
#include <cmath>
#include <optional>
#include <vector>

Camera::Camera() : forward_{config::initial_camera_forward}, position_{config::initial_camera_position} {
    if (std::abs(forward_[0]) < 1e-5f && std::abs(forward_[1]) < 1e-5f) {
        right_ = {1, 0, 0};
    } else {
        const float right_inv_mag = 1.0f / std::sqrt(forward_[0] * forward_[0] + forward_[1] * forward_[1]);
        right_ = {forward_[1] * right_inv_mag, -forward_[0] * right_inv_mag, 0};
    }
    up_ = {right_[1] * forward_[2] - right_[2] * forward_[1],
           right_[2] * forward_[0] - right_[0] * forward_[2],
           right_[0] * forward_[1] - right_[1] * forward_[0]};
    float up_inv_mag = 0.0f;
    for (int i = 0; i < 3; ++i) {
        up_inv_mag += up_[i] * up_[i];
    }
    up_inv_mag = 1.0f / std::sqrt(up_inv_mag);
    for (int i = 0; i < 3; ++i) {
        up_[i] *= up_inv_mag;
    }
}

std::array<float, 3> Camera::GetForward() const {
    return forward_;
}

std::array<float, 3> Camera::GetRight() const {
    return right_;
}

std::array<float, 3> Camera::GetUp() const {
    return up_;
}

std::array<float, 3> Camera::GetPosition() const {
    return position_;
}

std::array<float, 2> PixelToNormal(const std::array<int, 2>& pixel) {
    std::array<float, 2> result;
    result[0] = 2.0f * pixel[0] / config::window_size.x - 1.0f;
    result[1] = 1.0f - 2.0f * pixel[1] / config::window_size.y;
    return result;
}

std::vector<float> Rasterize(const Camera& camera, const std::vector<Triangle3D>& triangles) {
    std::vector<float> result(config::window_size.x * config::window_size.y, 0.0f);
    std::vector<float> depths(config::window_size.x * config::window_size.y, 1.0f);
    for (const auto& triangle : triangles) {
        const std::array<std::array<float, 3>, 3> vertices = triangle.GetVertices();
        std::array<float, 3> projected_y;
        std::array<float, 6> projected_vertices;
        bool clipped = false;
        for (int i = 0; i < 3; ++i) {
            const Math4D::Vector4 vertex_position = {{vertices[i][0], vertices[i][1], vertices[i][2], 1.0f}};
            const std::optional<Math4D::Vector4> clip_position = GetNDC(camera, vertex_position);
            if (!clip_position.has_value()) {
                clipped = true;
                break;
            }
            projected_y[i] = clip_position.value()[1];
            projected_vertices[2 * i] = clip_position.value()[0];
            projected_vertices[2 * i + 1] = clip_position.value()[2];
        }
        if (clipped) {
            continue;
        }
        const Triangle2D projected_triangle = projected_vertices;
        const std::array<float, 4> bounding_box = projected_triangle.GetBoundingBox();
        std::array<float, 4> bounding_box_px;
        bounding_box_px[0] = static_cast<uint32_t>(std::ceil(
            0.5f * (bounding_box[0] + 1.0f) * static_cast<float>(config::window_size.x)));
        bounding_box_px[1] = static_cast<uint32_t>(std::ceil(
            0.5f * (1.0f - bounding_box[1]) * static_cast<float>(config::window_size.y)));
        bounding_box_px[2] = static_cast<uint32_t>(std::floor(
            0.5f * (bounding_box[2] + 1.0f) * static_cast<float>(config::window_size.x)));
        bounding_box_px[3] = static_cast<uint32_t>(std::floor(
            0.5f * (1.0f - bounding_box[3]) * static_cast<float>(config::window_size.y)));
        for (int i = bounding_box_px[3]; i <= bounding_box_px[1]; ++i) {
            for (int j = bounding_box_px[0]; j <= bounding_box_px[2]; ++j) {
                std::array<float, 2> normal_position = PixelToNormal({j, i});
                std::array<float, 3> barycentric_coordinates = projected_triangle.GetBarycentricCoordinates(normal_position);
                if (!projected_triangle.IsInside(normal_position)) {
                    continue;
                }
                float cur_y = 0.0f;
                for (int i = 0; i < 3; ++i) {
                    cur_y += projected_y[i] * barycentric_coordinates[i];
                }
                if (cur_y >= depths[i * config::window_size.x + j]) {
                    continue;
                }
                depths[i * config::window_size.x + j] = cur_y;
                result[i * config::window_size.x + j] = (cur_y + 1.0f) * 0.5f;
            }
        }
    }
    return result;
}
