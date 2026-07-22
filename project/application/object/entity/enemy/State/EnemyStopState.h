#pragma once
/// ===Include=== ///
#include "Base/EnemyState.h"
#include <memory>

///-------------------------------------------/// 
/// EnemyStopState
/// Enemyの停止状態を管理するクラス
///-------------------------------------------///
class EnemyStopState : public EnemyState {
public:
	
	EnemyStopState(std::unique_ptr<EnemyState> state);

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

	float stopDuration_ = 0.8f; // 停止時間(秒)
	float stopTimer_ = 0.0f; // 経過時間

private:

	/// <summary>
	/// 一つ前の状態に戻る
	/// </summary>
	void ResumePreviousState();
};
				