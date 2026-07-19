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

void Camera::Move(const Math4D::Vector4& relative_direction) {
    const Math4D::Matrix4x4 camera_matrix = {{right_[0], forward_[0], up_[0], 0.0f,
                                              right_[1], forward_[1], up_[1], 0.0f,
                                              right_[2], forward_[2], up_[2], 0.0f,
                                              0.0f, 0.0f, 0.0f, 1.0f}};
    const Math4D::Vector4 direction = camera_matrix * relative_direction;
    for (int i = 0; i < 3; ++i) {
        position_[i] += direction[i];
    }
}

void Camera::LookAt(const Math4D::Vector4& relative_target_position) {
    if (std::abs(relative_target_position[0]) < 1e-5f && std::abs(relative_target_position[2]) < 1e-5f) {
        return;
    }
    const Math4D::Matrix4x4 camera_matrix = {{right_[0], forward_[0], up_[0], -position_[0],
                                              right_[1], forward_[1], up_[1], -position_[1],
                                              right_[2], forward_[2], up_[2], -position_[2],
                                              0.0f, 0.0f, 0.0f, 1.0f}};
    const Math4D::Vector4 target_direction = camera_matrix * relative_target_position;
    float forward_inv_mag = 0.0f;
    for (int i = 0; i < 3; ++i) {
        forward_[i] = target_direction[i];
        forward_inv_mag += forward_[i] * forward_[i];
    }
    forward_inv_mag = 1.0f / std::sqrt(forward_inv_mag);
    for (int i = 0; i < 3; ++i) {
        forward_[i] *= forward_inv_mag;
    }
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

std::array<float, 2> PixelToNormal(const std::array<int, 2>& pixel) {
    std::array<float, 2> result;
    result[0] = 2.0f * pixel[0] / config::window_size.x - 1.0f;
    result[1] = 1.0f - 2.0f * pixel[1] / config::window_size.y;
    return result;
}

void RasterizeTriangle(const Triangle2D& projected_triangle,
    std::array<float, 3>&& projected_y, std::vector<float>* const result, std::vector<float>* const depths) {
    const std::array<float, 4> bounding_box = projected_triangle.GetBoundingBox();
    std::array<int, 4> bounding_box_px;
    bounding_box_px[0] = static_cast<int>(std::ceil(
        0.5f * (bounding_box[0] + 1.0f) * static_cast<float>(config::window_size.x)));
    bounding_box_px[1] = static_cast<int>(std::ceil(
        0.5f * (1.0f - bounding_box[1]) * static_cast<float>(config::window_size.y)));
    bounding_box_px[2] = static_cast<int>(std::floor(
        0.5f * (bounding_box[2] + 1.0f) * static_cast<float>(config::window_size.x)));
    bounding_box_px[3] = static_cast<int>(std::floor(
        0.5f * (1.0f - bounding_box[3]) * static_cast<float>(config::window_size.y)));
    if (bounding_box_px[0] >= static_cast<int>(config::window_size.x) ||
        bounding_box_px[1] < 0 ||
        bounding_box_px[2] < 0 ||
        bounding_box_px[3] >= static_cast<int>(config::window_size.y)) {
        return;
    }
    bounding_box_px[0] = std::max(0, bounding_box_px[0]);
    bounding_box_px[1] = std::min(static_cast<int>(config::window_size.y) - 1, bounding_box_px[1]);
    bounding_box_px[2] = std::min(static_cast<int>(config::window_size.x) - 1, bounding_box_px[2]);
    bounding_box_px[3] = std::max(0, bounding_box_px[3]);
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
            if ((*depths)[i * config::window_size.x + j] > 1.0f || cur_y >= (*depths)[i * config::window_size.x + j]) {
                continue;
            }
            (*depths)[i * config::window_size.x + j] = cur_y;
            (*result)[i * config::window_size.x + j] = 1.0f;
        }
    }
}

std::vector<float> Rasterize(const Camera& camera, const std::vector<Triangle3D>& triangles) {
    std::vector<float> result(config::window_size.x * config::window_size.y, 0.0f);
    std::vector<float> depths(config::window_size.x * config::window_size.y, 1.0f);
    for (const auto& triangle : triangles) {
        const std::array<std::array<float, 3>, 3> vertices = triangle.GetVertices();
        std::array<Math4D::Vector4, 3> vertex_positions;
        std::array<Math4D::Vector4, 3> clip_positions;
        std::vector<int> clipped_indexes;
        for (int i = 0; i < 3; ++i) {
            vertex_positions[i] = {{vertices[i][0], vertices[i][1], vertices[i][2], 1.0f}};
            clip_positions[i] = GetClipPosition(camera, vertex_positions[i]);
            if (clip_positions[i][3] < config::near_plane_distance) {
                clipped_indexes.push_back(i);
            }
        }
        if (clipped_indexes.size() == 3) {
            continue;
        }
        if (clipped_indexes.size() == 2) {
            const int good_index = 3 - clipped_indexes[0] - clipped_indexes[1];
            for (auto clipped_index : clipped_indexes) {
                const float t = (config::near_plane_distance - clip_positions[clipped_index][3]) /
                                (clip_positions[good_index][3] - clip_positions[clipped_index][3]);
                clip_positions[clipped_index] = Math4D::Lerp(clip_positions[clipped_index],
                    clip_positions[good_index], t);
            }
            std::array<float, 3> projected_y;
            std::array<float, 6> projected_vertices;
            for (int i = 0; i < 3; ++i) {
                const Math4D::Vector4 ndc = GetNDC(clip_positions[i]);
                projected_y[i] = ndc[1];
                projected_vertices[2 * i] = ndc[0];
                projected_vertices[2 * i + 1] = ndc[2];
            }
            const Triangle2D projected_triangle = projected_vertices;
            RasterizeTriangle(projected_triangle, std::move(projected_y), &result, &depths);
            continue;
        }
        if (clipped_indexes.size() == 1) {
            const int clipped_index = clipped_indexes[0];
            const std::array<int, 2> good_indexes = {(clipped_index + 1) % 3, (clipped_index + 2) % 3};
            std::array<Math4D::Vector4, 2> clipped_positions;
            for (int i = 0; i < 2; ++i) {
                const int good_index = good_indexes[i];
                const float t = (config::near_plane_distance - clip_positions[clipped_index][3]) /
                                (clip_positions[good_index][3] - clip_positions[clipped_index][3]);
                clipped_positions[i] = Math4D::Lerp(clip_positions[clipped_index],
                    clip_positions[good_index], t);
            }

            {
                std::array<float, 3> projected_y;
                std::array<float, 6> projected_vertices;

                Math4D::Vector4 ndc = GetNDC(clip_positions[good_indexes[0]]);
                projected_y[0] = ndc[1];
                projected_vertices[0] = ndc[0];
                projected_vertices[1] = ndc[2];

                ndc = GetNDC(clip_positions[good_indexes[1]]);
                projected_y[1] = ndc[1];
                projected_vertices[2] = ndc[0];
                projected_vertices[3] = ndc[2];
                
                ndc = GetNDC(clipped_positions[0]);
                projected_y[2] = ndc[1];
                projected_vertices[4] = ndc[0];
                projected_vertices[5] = ndc[2];

                const Triangle2D projected_triangle = projected_vertices;
                RasterizeTriangle(projected_triangle, std::move(projected_y), &result, &depths);
            }

            {
                std::array<float, 3> projected_y;
                std::array<float, 6> projected_vertices;

                Math4D::Vector4 ndc = GetNDC(clip_positions[good_indexes[1]]);
                projected_y[0] = ndc[1];
                projected_vertices[0] = ndc[0];
                projected_vertices[1] = ndc[2];

                ndc = GetNDC(clipped_positions[1]);
                projected_y[1] = ndc[1];
                projected_vertices[2] = ndc[0];
                projected_vertices[3] = ndc[2];
                
                ndc = GetNDC(clipped_positions[0]);
                projected_y[2] = ndc[1];
                projected_vertices[4] = ndc[0];
                projected_vertices[5] = ndc[2];

                const Triangle2D projected_triangle = projected_vertices;
                RasterizeTriangle(projected_triangle, std::move(projected_y), &result, &depths);
            }

            continue;
        }
        if (clipped_indexes.size() == 0) {
            std::array<float, 3> projected_y;
            std::array<float, 6> projected_vertices;
            for (int i = 0; i < 3; ++i) {
                const Math4D::Vector4 ndc = GetNDC(clip_positions[i]);
                projected_y[i] = ndc[1];
                projected_vertices[2 * i] = ndc[0];
                projected_vertices[2 * i + 1] = ndc[2];
            }
            const Triangle2D projected_triangle = projected_vertices;
            RasterizeTriangle(projected_triangle, std::move(projected_y), &result, &depths);
            continue;
        }
    }
    return result;
}
