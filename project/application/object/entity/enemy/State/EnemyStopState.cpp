#include "EnemyStopState.h"
// Enemy
#include "application/object/entity/enemy/BaseEnemy.h"
// state
#include "EnemyMoveState.h"
// Math
#include <MyMath.h>

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
EnemyStopState::EnemyStopState(std::unique_ptr<EnemyState> state) {
	previousState_ = std::move(state);
}

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
		enemy_->ChangeState(std::move(previousState_));
	}
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void EnemyStopState::Exit() {
	EnemyState::Exit();
}