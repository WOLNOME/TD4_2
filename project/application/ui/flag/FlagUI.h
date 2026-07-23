#pragma once

#include "engine/2d/Sprite.h"
#include "engine/3d/worldTransform/WorldTransform.h"
#include "engine/3d/camera/BaseCamera.h"
#include "engine/input/Input.h"

#include <memory>

class FlagUI {

public:

	void Initialize(
		const std::string _uiName,
		const std::string _onTextureName,
		const std::string _offTextureName,
		const Norm::Vector3 _offset,
		const Norm::Vector2 _size,
		const BYTE _keyNumber,
		const bool _isMouseUI,
		Norm::BaseCamera* _camera,
		Norm::Input* _input
	);

	void Update(Norm::Vector3 _playerPos, Norm::Vector4 _color);

private:

	Norm::BaseCamera* camera_ = nullptr;

	Norm::Input* input_ = nullptr;

	int32_t onFlagTexture_;

	int32_t offFlagTexture_;

	std::unique_ptr<Norm::Sprite> sprite_ = nullptr;

	Norm::WorldTransform worldTransform_;

	bool isFlag_ = false;

	bool isMouseUI_ = false;

	BYTE keyNumber_;

	Norm::Vector3 offset_ = { 0.0f,0.0f,0.0f };

	Norm::Vector2 size_ = { 32.0f,32.0f };
};