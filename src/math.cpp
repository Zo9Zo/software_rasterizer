#include "math.hpp"

#include <array>
#include <cmath>

float Math4D::Matrix4x4::operator[](int i) const {
    return this->data_[i];
}

float& Math4D::Matrix4x4::operator[](int i) {
    return this->data_[i];
}

float Math4D::Vector4::operator[](int i) const {
    return this->data_[i];
}

float& Math4D::Vector4::operator[](int i) {
    return this->data_[i];
}

Math4D::Vector4 Math4D::Vector4::operator*(float a) const {
    Vector4 result = *this;
    for (int i = 0; i < 4; ++i) {
        result[i] *= a;
    }
    return result;
}

Math4D::Vector4 Math4D::Vector4::operator+(const Math4D::Vector4& other) const {
    Vector4 result = *this;
    result += other;
    return result;
}

Math4D::Vector4 Math4D::Vector4::operator-(const Math4D::Vector4& other) const {
    Vector4 result = *this;
    result += other * (-1.0f);
    return result;
}

Math4D::Vector4 Math4D::Lerp(const Math4D::Vector4& v1, const Math4D::Vector4& v2, float t) {
    return v1 + (v2 - v1) * t;
}

Math4D::Vector4& Math4D::Vector4::operator+=(const Vector4& other) {
    for (int i = 0; i < 4; ++i) {
        (*this)[i] += other[i];
    }
    return *this;
}

Math4D::Vector4& Math4D::Vector4::operator/=(float a) {
    const float inv_a = 1.0f / a;
    for (int i = 0; i < 4; ++i) {
        data_[i] *= inv_a;
    }
    return *this;
}

float Math4D::Vector4::Magnitude() const {
    float sqr_magnitude = 0.0f;
    for (int i = 0; i < 4; ++i) {
        sqr_magnitude += data_[i] * data_[i];
    }
    return std::sqrt(sqr_magnitude);
}

void Math4D::Vector4::Normalize() {
    *this /= this->Magnitude();
}

Math4D::Vector4 Math4D::operator*(const Matrix4x4& mat, const Vector4& vec) {
    Vector4 result;
    for (int i = 0; i < 4; ++i) {
        result[i] = 0;
        for (int j = 0; j < 4; ++j) {
            result[i] += mat[i * 4 + j] * vec[j];
        }
    }
    return result;
}

Math4D::Matrix4x4 Math4D::operator*(const Matrix4x4& mat1, const Matrix4x4& mat2) {
    Matrix4x4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i * 4 + j] = 0;
            for (int k = 0; k < 4; ++k) {
                result[i * 4 + j] += mat1[i * 4 + k] * mat2[k * 4 + j];
            }
        }
    }
    return result;
}
