#pragma once
/// ===Include=== ///
#include "Base/EnemyState.h"

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

private:
	/// ============================== ///
	///		メンバ変数
	/// ============================== ///
	// 移動速度
	float moveSpeed_ = 0.05f;

private:

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// 状態遷移の確認
	/// </summary>
	void CheckTransition();
};

