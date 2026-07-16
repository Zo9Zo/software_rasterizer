#include "geometry.hpp"

#include <algorithm>
#include <array>
#include <cmath>

Triangle2D::Triangle2D(const std::array<float, 6>& vertices) {
    v1_ = {vertices[0], vertices[1]};
    v2_ = {vertices[2], vertices[3]};
    v3_ = {vertices[4], vertices[5]};
}

std::array<std::array<float, 2>, 3> Triangle2D::GetVertices() const {
    return {v1_, v2_, v3_};
}

float Triangle2D::GetSignedDoubleArea() const {
    return (v2_[0] - v1_[0]) * (v3_[1] - v1_[1]) - (v2_[1] - v1_[1]) * (v3_[0] - v1_[0]);
}

std::array<float, 3> Triangle2D::GetBarycentricCoordinates(const std::array<float, 2>& position) const {
    std::array<float, 3> crosses;
    const float inv_double_area = 1.0f / GetSignedDoubleArea();
    const std::array<const std::array<float, 2>, 3> edges = {{{v3_[0] - v2_[0], v3_[1] - v2_[1]},
                                                              {v1_[0] - v3_[0], v1_[1] - v3_[1]},
                                                              {v2_[0] - v1_[0], v2_[1] - v1_[1]}}};
    crosses[0] = ((position[1] - v2_[1]) * edges[0][0] -
                  (position[0] - v2_[0]) * edges[0][1]) * inv_double_area;
    crosses[1] = ((position[1] - v3_[1]) * edges[1][0] -
                  (position[0] - v3_[0]) * edges[1][1]) * inv_double_area;
    crosses[2] = ((position[1] - v1_[1]) * edges[2][0] -
                  (position[0] - v1_[0]) * edges[2][1]) * inv_double_area;
    return crosses;
}

bool Triangle2D::IsInside(const std::array<float, 2>& position) const {
    std::array<float, 3> barycentric_coordinates = GetBarycentricCoordinates(position);
    return barycentric_coordinates[0] >= 0 &&
           barycentric_coordinates[1] >= 0 &&
           barycentric_coordinates[2] >= 0;
}

bool Triangle2D::IsInside(const std::array<float, 3>& barycentric_coordinates) const {
    return barycentric_coordinates[0] >= 0 &&
           barycentric_coordinates[1] >= 0 &&
           barycentric_coordinates[2] >= 0;
}

std::array<float, 4> Triangle2D::GetBoundingBox() const {
    std::array<float, 4> bounding_box;
    bounding_box[0] = std::min({v1_[0], v2_[0], v3_[0]});
    bounding_box[1] = std::min({v1_[1], v2_[1], v3_[1]});
    bounding_box[2] = std::max({v1_[0], v2_[0], v3_[0]});
    bounding_box[3] = std::max({v1_[1], v2_[1], v3_[1]});
    return bounding_box;
}

Triangle3D::Triangle3D(const std::array<float, 9>& vertices) {
    v1_ = {vertices[0], vertices[1], vertices[2]};
    v2_ = {vertices[3], vertices[4], vertices[5]};
    v3_ = {vertices[6], vertices[7], vertices[8]};
}

std::array<std::array<float, 3>, 3> Triangle3D::GetVertices() const {
    return {v1_, v2_, v3_};
}