#pragma once
/// ===include=== ///
#include "Base/EnemyState.h"
#include "Vector4.h"

///=====================================================/// 
/// EnemyDeadState
/// Enemyの死亡状態を管理するクラス
///=====================================================///
class EnemyDeadState : public EnemyState {
public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Enter(BaseEnemy* enemy) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Exit() override;

private:
	// 死亡アニメーションが終了したかどうかのフラグ
	bool isFinishedAnimation_ = false; 

	// アニメーションのパラメータ
	struct AnimationParameters {
		float rotationSpeed = 0.2f;   // 回転速度
		float jumpPower = 0.55f;      // ジャンプの高さ
		float gravity = -0.03f;		  // 重力の影響
		float colorFadeSpeed = 0.02f; // 色のフェード速度
		float moveSpeedX = 0.04f;    // X軸方向の移動速度
		float moveSpeedZ = 0.05f;    // Z軸方向の移動速度
	};
	AnimationParameters parames_ = {};

	// Y軸の速度
	float yVelocity_ = 0.0f; 

private:

	/// <summary>
	/// 死亡アニメーションの処理
	/// </summary>
	void DeadAnimation();
};

