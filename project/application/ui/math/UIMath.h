#pragma once

#include "engine/math/Vector2.h"
#include "engine/math/Vector3.h"
#include "engine/math/Matrix4x4.h"

Norm::Vector2 WorldToScreen(const Norm::Vector3 _pos, const Norm::Matrix4x4 _viewProjectionMatrix);

Norm::Vector3 ScreenToWorld(const Norm::Vector2 _pos, const Norm::Vector3 _cameraPos, const Norm::Matrix4x4 _viewProjection);

