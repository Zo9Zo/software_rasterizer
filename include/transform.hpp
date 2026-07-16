#pragma once

#include "rasterizer.hpp"
#include "math.hpp"

Math4D::Matrix4x4 GetModelToWorldMatrix();

Math4D::Matrix4x4 GetWorldToViewMatrix(const Camera& camera);

Math4D::Matrix4x4 GetViewToProjectionMatrix();

std::optional<Math4D::Vector4> GetNDC(const Camera& camera, const Math4D::Vector4& model_position);
