#pragma once
/// ===Include=== ///
#include "Base/EnemyState.h"
#include <Vector3.h>
#include <memory>

///=====================================================/// 
/// EnemyKnockbackState
/// Enemyのノックバック状態を管理するクラス
///=====================================================///
class EnemyKnockbackState : public EnemyState {
public:

	EnemyKnockbackState(std::unique_ptr<EnemyState> state);

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

	// 前の状態へのポインタ
	std::unique_ptr<EnemyState> previousState_ = nullptr;

	Norm::Vector3 knockbackDirection_ = { 0.0f, 0.0f, 0.0f };

	// ノックバックの速度
	Norm::Vector3 knockbackVelocity_ = { 0.0f, 0.0f, 0.0f };
	// ノックバックの減衰率
	float knockbackDamping_ = 0.9f;
	// ノックバックの速度
	float KnockbackSpeed_ = 0.4f;

private:

	/// <summary>
	/// ノックバック処理
	/// </summary>
	void KnockBack();
};

