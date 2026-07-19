#pragma once
/// ===include=== ///
#include "Base/EnemyState.h"
#include "Vector3.h"

///=====================================================/// 
/// EnemyEscapeState
/// Enemyの逃走状態を管理するクラス
///=====================================================///
class EnemyEscapeState : public EnemyState {
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
	/// ============================== ///
	///		メンバ変数
	/// ============================== ///

	// Enterで決定したランダムな逃走方向
	Norm::Vector3 escapeDirection_{};

	// この状態にとどまる時間(秒)
	float escapeDuration_ = 0.5f;
	// 経過時間
	float escapeTimer_ = 0.0f;

	// 逃走時の移動速度
	float escapeSpeed_ = 0.4f;
};