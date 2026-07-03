#pragma once
/// ===Include=== ///
#include "EnemyState.h"

///=====================================================/// 
/// EnemyMoveState
/// Enemyの移動状態を管理するクラス
///=====================================================///
class EnemyMoveState : public EnemyState {
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
};

