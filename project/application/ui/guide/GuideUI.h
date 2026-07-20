#pragma once

#include "engine/2d/Sprite.h"
#include "engine/3d/worldTransform/WorldTransform.h"
#include "engine/3d/camera/BaseCamera.h"
#include "engine/input/Input.h"

class GuideUI {
public:
	/// ============================== ///
	///		メンバ関数
	/// ============================== ///

	void Initialize(Norm::BaseCamera* _camera, Norm::Input* _input, Norm::Vector3 _pos);

	void Update();

	void ImGui();

	void SetPosition(const Norm::Vector3 _pos) { mouseWorldTransform_.SetTranslate(_pos); }

private:
	/// ============================== ///
	///		クラス内関数
	/// ============================== ///

	void UpdateSpritePos();

	float GetDistanceToMouse();

private:
	/// ============================== ///
	///		メンバ変数
	/// ============================== ///

	int32_t mouseTextureHandle_ = EOF;
	int32_t clickMouseTextureHandle_ = EOF;
	std::unique_ptr<Norm::Sprite> mouseSprite_ = nullptr;
	Norm::WorldTransform mouseWorldTransform_;

	int32_t clickTextureHandle_ = EOF;
	std::unique_ptr<Norm::Sprite> clickSprite_ = nullptr;

	Norm::BaseCamera* camera_;
	Norm::Input* input_;

	float acceptableLange_ = 4.0f;
	float alpha_ = 0.0f;
	float alphaSpeed_ = 0.1f;

	float textureChangeTimer_ = 0.0f;
	float textureChangeMaxTime_ = 0.5f;

	bool isClickTexture_ = false;

	Norm::Vector4 spriteColor_ = { 0.5f,0.5f,0.5f,1.0f };

	Norm::Vector3 clickSpriteOffset_ = { -16.0f,-20.0f,0.0f };
};

