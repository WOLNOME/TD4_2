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

	int32_t textureHandle_ = EOF;
	std::unique_ptr<Norm::Sprite> sprite_ = nullptr;
	Norm::WorldTransform worldTransform_;
	Norm::BaseCamera* camera_;
	Norm::Input* input_;

	float acceptableLange_ = 4.0f;
	float alpha_ = 0.0f;
	float alphaSpeed_ = 0.1f;

	Norm::Vector4 spriteColor_ = { 0.5f,0.5f,0.5f,1.0f };

};

