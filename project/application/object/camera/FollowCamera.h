#pragma once

// Engine
#include <MyMath.h>
#include <BaseCamera.h>

namespace Norm {
class Player; // 前方宣言

class FollowCamera : public BaseCamera {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="target"></param>
	void Initialize(const Player* target);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void DebugWithImGui();

private:
	// 追従対象
	const Player* target_ = nullptr;

	// カメラのパラメーター
	Vector3 offset_ = {0.0f, 0.0f, -50.0f}; // プレイヤーからの相対位置
	float interpolationRate_ = 0.15f; // 補間係数（0.0~1.0f）
};
}