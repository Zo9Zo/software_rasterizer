#pragma once

#include <array>

class Triangle2D {
   public:
    Triangle2D(const std::array<float, 2>& v1, const std::array<float, 2>& v2,
        const std::array<float, 2>& v3) : v1_{v1}, v2_{v2}, v3_{v3} {
    }
    
    Triangle2D(const std::array<float, 6>& vertices);

    std::array<std::array<float, 2>, 3> GetVertices() const;

    float GetSignedDoubleArea() const;

    std::array<float, 3> GetBarycentricCoordinates(const std::array<float, 2>& position) const;

    bool IsInside(const std::array<float, 2>& position) const;
    
    bool IsInside(const std::array<float, 3>& barycentric_coordinates) const;
    
    std::array<float, 4> GetBoundingBox() const;

   private:
    std::array<float, 2> v1_;
    std::array<float, 2> v2_;
    std::array<float, 2> v3_;
};

class Triangle3D {
   public:
    Triangle3D(const std::array<float, 3>& v1, const std::array<float, 3>& v2,
        const std::array<float, 3>& v3) : v1_{v1}, v2_{v2}, v3_{v3} {
    }

    Triangle3D(const std::array<float, 9>& vertices);

    std::array<std::array<float, 3>, 3> GetVertices() const;

   private:
    std::array<float, 3> v1_;
    std::array<float, 3> v2_;
    std::array<float, 3> v3_;
};
