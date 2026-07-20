#pragma once
/// ===Include=== ///
#include "Base/EnemyState.h"
#include "Vector2.h"

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

	// 視野の範囲
	Norm::Vector2 viewRange_ = { 10.0f, 10.0f };

private:

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// 状態遷移の確認
	/// </summary>
	bool CheckTransition();
};

