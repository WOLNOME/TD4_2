#pragma once

#include "engine/2d/Sprite.h"
#include "engine/3d/worldTransform/WorldTransform.h"
#include "engine/3d/camera/BaseCamera.h"

class GuideUI {
public:
	/// ============================== ///
	///		メンバ関数
	/// ============================== ///

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Norm::BaseCamera* _camera);

	void Update();

private:
	/// ============================== ///
	///		メンバ変数
	/// ============================== ///

	int32_t textureHandle_ = EOF;
	std::unique_ptr<Norm::Sprite> sprite_ = nullptr;
	Norm::WorldTransform worldTransform_;
	Norm::BaseCamera* camera_;

};

