#pragma once
/// ===include=== ///
#include "EnemyState.h"

///=====================================================/// 
/// EnemyAttackState
/// Enemyの攻撃状態を管理するクラス
///=====================================================///
class EnemyAttackState : public EnemyState {
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

