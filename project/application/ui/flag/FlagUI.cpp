#include "FlagUI.h"

#include "engine/base/windows/WinApp.h"
#include "engine/base/texture/TextureManager.h"
#include "engine/2d/SpriteManager.h"
#include "application/ui/math/UIMath.h"

using namespace Norm;

void FlagUI::Initialize(
	const std::string _uiName,
	const std::string _onTextureName,
	const std::string _offTextureName,
	const Norm::Vector3 _offset,
	const BYTE _keyNumber,
	const bool _isMouseUI,
	Norm::BaseCamera* _camera,
	Norm::Input* _input
) {

	camera_ = _camera;

	input_ = _input;

	keyNumber_ = _keyNumber;

	isMouseUI_ = _isMouseUI;

	onFlagTexture_ = TextureManager::GetInstance()->LoadTexture(_onTextureName);

	offFlagTexture_ = TextureManager::GetInstance()->LoadTexture(_offTextureName);

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName(_uiName), Order::Front2, offFlagTexture_);
	sprite_->SetAnchorPoint({ 0.5f,0.5f });
	sprite_->SetPosition({ 0.0f,0.0f });

	worldTransform_.Initialize();

	offset_ = _offset;
}

void FlagUI::Update(Norm::Vector3 _playerPos) {

	if (isMouseUI_) {

		if (input_->PushMouseButton(MouseButton::LeftButton)) {

			isFlag_ = true;
		} else {

			isFlag_ = false;
		}
	} else {

		if (input_->PushKey(keyNumber_)) {

			isFlag_ = true;
		} else {

			isFlag_ = false;
		}
	}

	if (isFlag_) {

		sprite_->SetTexture(onFlagTexture_);
	} else {

		sprite_->SetTexture(offFlagTexture_);
	}

	worldTransform_.SetTranslate(_playerPos + offset_);

	worldTransform_.UpdateMatrix();

	sprite_->SetPosition(WorldToScreen(worldTransform_.GetTranslate(), camera_->GetViewProjectionMatrix()));

	sprite_->SetSize(size_);
}