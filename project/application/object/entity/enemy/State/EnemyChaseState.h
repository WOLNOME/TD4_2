#pragma once
/// ===include=== ///
#include "Base/EnemyState.h"

///=====================================================/// 
/// EnemyChaseState
/// Enemyの追跡状態を管理するクラス
///=====================================================///
class EnemyChaseState : public EnemyState {
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

	// 追跡関連の変数
	float chaseSpeed_ = 0.1f; // 追跡速度

private:

	/// <summary>
	/// プレイヤーを追う処理
	/// </summary>
	void ChasePlayer();
};

