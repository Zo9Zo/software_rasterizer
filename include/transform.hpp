#pragma once

#include "rasterizer.hpp"
#include "math.hpp"

Math4D::Matrix4x4 GetModelToWorldMatrix();

Math4D::Matrix4x4 GetWorldToViewMatrix(const Camera& camera);

Math4D::Matrix4x4 GetViewToProjectionMatrix();

Math4D::Vector4 GetClipPosition(const Camera& camera, const Math4D::Vector4& model_position);

Math4D::Vector4 GetNDC(const Math4D::Vector4& clip_position);
