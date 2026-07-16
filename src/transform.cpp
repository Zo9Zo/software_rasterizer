#include "transform.hpp"
#include "rasterizer.hpp"
#include "config.hpp"
#include "math.hpp"

#include <array>
#include <cmath>
#include <optional>

Math4D::Matrix4x4 GetModelToWorldMatrix() {
    return Math4D::Identity;
}

Math4D::Matrix4x4 GetWorldToViewMatrix(const Camera& camera) {
    const std::array<float, 3> camera_forward = camera.GetForward();
    const std::array<float, 3> camera_right = camera.GetRight();
    const std::array<float, 3> camera_up = camera.GetUp();
    const std::array<float, 3> camera_position = camera.GetPosition();

    std::array<float, 3> dots{};
    for (int i = 0; i < 3; ++i) {
        dots[0] += camera_right[i] * camera_position[i];
        dots[1] += camera_forward[i] * camera_position[i];
        dots[2] += camera_up[i] * camera_position[i];
    }

    return {{camera_right[0], camera_right[1], camera_right[2], -dots[0],
             camera_forward[0], camera_forward[1], camera_forward[2], -dots[1],
             camera_up[0], camera_up[1], camera_up[2], -dots[2],
             0.0f, 0.0f, 0.0f, 1.0f}};
}

Math4D::Matrix4x4 GetViewToProjectionMatrix() {
    const float one_over_tan_half_fov = 1.0f / static_cast<float>(std::tan(config::fov.asRadians() * 0.5f));

    return {{one_over_tan_half_fov / config::aspect_ratio, 0.0f, 0.0f, 0.0f,
             0.0f, (config::far_plane_distance + config::near_plane_distance) /
             (config::far_plane_distance - config::near_plane_distance), 0.0f,
             -2.0f * config::far_plane_distance * config::near_plane_distance /
             (config::far_plane_distance - config::near_plane_distance),
             0.0f, 0.0f, one_over_tan_half_fov, 0.0f,
             0.0f, 1.0f, 0.0f, 0.0f}};
}

Math4D::Matrix4x4 GetMVPMatrix(const Camera& camera) {
    return GetViewToProjectionMatrix() * GetWorldToViewMatrix(camera) * GetModelToWorldMatrix();
}

std::optional<Math4D::Vector4> GetClipPosition(const Camera& camera, const Math4D::Vector4& model_position) {
    Math4D::Vector4 clip_position = GetMVPMatrix(camera) * model_position;
    if (clip_position[3] <= 1e-5f) {
        return std::nullopt;
    }
    for (int i = 0; i < 3; ++i) {
        if (std::abs(clip_position[i]) > clip_position[3]) {
            return std::nullopt;
        }
    }
    return clip_position;
}

std::optional<Math4D::Vector4> GetNDC(const Camera& camera, const Math4D::Vector4& model_position) {
    std::optional<Math4D::Vector4> clip_position = GetClipPosition(camera, model_position);
    if (!clip_position.has_value()) {
        return std::nullopt;
    }
    const float w = clip_position.value()[3];
    clip_position.value() /= w;
    return clip_position;
}
