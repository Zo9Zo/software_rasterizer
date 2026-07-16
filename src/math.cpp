#include "math.hpp"

#include <array>

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

Math4D::Vector4& Math4D::Vector4::operator/=(float a) {
    const float inv_a = 1.0f / a;
    for (int i = 0; i < 4; ++i) {
        data_[i] *= inv_a;
    }
    return *this;
}