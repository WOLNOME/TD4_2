#pragma once
#include "BaseCamera.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include <memory>

/// <summary>
/// シーン上の地面の配置を行うクラス
/// </summary>
class Ground {
public:
	/// ============================== ///
	///		メンバ関数
	/// ============================== ///

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

private:
	/// ============================== ///
	///		メンバ変数
	/// ============================== ///

	int32_t textureHandle_ = EOF;
	std::unique_ptr<Norm::Object3d> object3d_ = nullptr;
	Norm::WorldTransform worldTransform_;

};

