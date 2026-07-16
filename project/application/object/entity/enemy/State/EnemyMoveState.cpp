#include "EnemyMoveState.h"
#include "application/object/entity/enemy/BaseEnemy.h"

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void EnemyMoveState::Enter(BaseEnemy* enemy) {
	enemy_ = enemy;
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void EnemyMoveState::Update() {
	/// ===タイマーの更新=== ///

	/// ===移動処理=== ///
	Move();

	/// ===Stateの移動=== ///
	CheckTransition();
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void EnemyMoveState::Exit() {
	EnemyState::Exit();
}

///-------------------------------------------/// 
/// 移動処理
///-------------------------------------------///
void EnemyMoveState::Move() {
#ifdef _DEBUG

	// フラグがtrueなら180度回転させる
	if (enemy_->GetIsTurning()) {
		enemy_->SetCurrentDirection(Opposite(enemy_->GetCurrentDirection()));
		enemy_->SetIsTurning(false);
	}
#endif // _DEBUG

	// 進行方向への速度を設定
	float sign = DirectionToSign(enemy_->GetCurrentDirection());
	enemy_->SetVelocity({ sign * moveSpeed_, 0.0f, 0.0f });

	// 向きの更新
	enemy_->UpdateFacing(sign);
}
 
///-------------------------------------------/// 
/// 状態遷移の確認
///-------------------------------------------///
void EnemyMoveState::CheckTransition() {
	// 視界の範囲内にPlayerがいるかどうかを確認。

}
