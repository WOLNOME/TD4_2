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

	void Update(const Norm::Vector3 _pos, const float _ratio);

	void ImGui();

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

	int32_t timerTextureHandle_ = EOF;
	std::unique_ptr<Norm::Sprite> timerSprite_ = nullptr;

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

	Norm::Vector3 timerSpriteOffset_ = { 0.0f,-20.0f };

	Norm::Vector2 timerSpriteSize_ = { 80.0f,8.0f };

};

