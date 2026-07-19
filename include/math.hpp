#pragma once

#include <array>
#include <compare>

namespace Math4D {
    class Matrix4x4 {
       public:
        Matrix4x4() = default;
        
        Matrix4x4(const Matrix4x4&) = default;
        
        Matrix4x4& operator=(const Matrix4x4&) = default;
        
        Matrix4x4(Matrix4x4&&) = default;
        
        Matrix4x4& operator=(Matrix4x4&&) = default;

        constexpr Matrix4x4(const std::array<float, 16>& data) : data_{data} {
        }

        float operator[](int i) const;
            
        float& operator[](int i);

       private:
        std::array<float, 16> data_;
    };
    
    class Vector4 {
       public:
        Vector4() = default;
        
        Vector4(const Vector4&) = default;
        
        Vector4& operator=(const Vector4&) = default;
        
        Vector4(Vector4&&) = default;
        
        Vector4& operator=(Vector4&&) = default;

        constexpr Vector4(const std::array<float, 4>& data) : data_{data} {
        }

        float operator[](int i) const;
            
        float& operator[](int i);

        Vector4 operator*(float a) const;

        Vector4 operator+(const Vector4& other) const;
        
        Vector4 operator-(const Vector4& other) const;

        Vector4& operator+=(const Vector4& other);
        
        Vector4& operator/=(float a);
        
        auto operator<=>(const Vector4&) const = default;

        float Magnitude() const;

        void Normalize();

       private:
        std::array<float, 4> data_;
    };

    Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t);

    Vector4 operator*(const Matrix4x4& mat, const Vector4& vec);
    
    Matrix4x4 operator*(const Matrix4x4& mat1, const Matrix4x4& mat2);

    inline constexpr Matrix4x4 Identity = Matrix4x4({1.0f, 0.0f, 0.0f, 0.0f,
                                                     0.0f, 1.0f, 0.0f, 0.0f,
                                                     0.0f, 0.0f, 1.0f, 0.0f,
                                                     0.0f, 0.0f, 0.0f, 1.0f});
}