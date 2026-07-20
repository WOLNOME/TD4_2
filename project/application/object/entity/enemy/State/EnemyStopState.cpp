#include "EnemyStopState.h"
// Enemy
#include "application/object/entity/enemy/BaseEnemy.h"
// State
#include "EnemyChaseState.h"


///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
EnemyStopState::EnemyStopState(std::unique_ptr<EnemyState> state) {
	// 所有権を受け取る
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
	stopTimer_ += 1.0f / 60.0f; // 60FPS想定で減算

	/// ===Stateの変更=== ///
	if (stopTimer_ >= stopDuration_) {
		ResumePreviousState(); // 前の状態に戻る
	}
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void EnemyStopState::Exit() {
	EnemyState::Exit();
}

///-------------------------------------------/// 
/// 一つ前の状態に戻る
///-------------------------------------------///
void EnemyStopState::ResumePreviousState() {
	if (previousState_) {
		enemy_->ChangeState(std::move(previousState_));
	}
}
