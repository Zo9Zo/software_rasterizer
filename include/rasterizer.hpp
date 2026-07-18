#pragma once

#include "geometry.hpp"
#include "math.hpp"

#include <array>
#include <optional>
#include <vector>

class Camera {
   public:
    Camera();

    std::array<float, 3> GetForward() const;
    
    std::array<float, 3> GetRight() const;
    
    std::array<float, 3> GetUp() const;
    
    std::array<float, 3> GetPosition() const;

    void Move(const Math4D::Vector4& relative_direction);
    
    void LookAt(const Math4D::Vector4& relative_target_position);

   private:
    std::array<float, 3> forward_;
    std::array<float, 3> right_;
    std::array<float, 3> up_;
    std::array<float, 3> position_;
};

std::array<float, 2> PixelToNormal(const std::array<int, 2>& pixel);

std::vector<float> Rasterize(const Camera& camera, const std::vector<Triangle3D>& triangles);
