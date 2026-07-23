#include "EnemyStopState.h"
// Enemy
#include "application/object/entity/enemy/BaseEnemy.h"
// state
#include "EnemyMoveState.h"
// Math
#include <MyMath.h>

///-------------------------------------------/// \
/// 初期化処理
///-------------------------------------------///
void EnemyStopState::Enter(BaseEnemy* enemy) {
	enemy_ = enemy;
	stopTimer_ = 0.0f; // 停止時間をリセット
	enemy_->SetVelocity({ 0.0f, 0.0f, 0.0f }); // 停止中は速度をゼロにする
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void EnemyStopState::Update() {
	/// ===タイマーの更新=== ///
	stopTimer_ += Norm::kDeltaTime;

	/// ===Stateの変更=== ///
	if (stopTimer_ >= stopDuration_) {
		// 終了時にコライダーを再設定
		enemy_->SetBodyCollider();
		// 移動状態に遷移
		enemy_->ChangeState(std::make_unique<EnemyMoveState>());
	}
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void EnemyStopState::Exit() {
	EnemyState::Exit();
}