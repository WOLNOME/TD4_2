#pragma once
/// ===Include=== ///
#include "Base/EnemyState.h"

///-------------------------------------------/// 
/// EnemyStopState
/// Enemyの停止状態を管理するクラス
///-------------------------------------------///
class EnemyStopState : public EnemyState {
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

	float stopDuration_ = 0.8f; // 停止時間
	float stopTimer_ = 0.0f; // 経過時間
};
				