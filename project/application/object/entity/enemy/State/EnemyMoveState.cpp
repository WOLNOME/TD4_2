#include "EnemyMoveState.h"
// Enemy
#include "application/object/entity/enemy/BaseEnemy.h"
// Player
#include <application/object/Character/Player.h>
// State
#include "EnemyChaseState.h"
// Math
#include <cmath>

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
	/// ===移動処理=== ///
	Move();

	/// ===Stateの移動=== ///
	if (CheckTransition()) {
		// Playerを発見したのでAlert状態へ遷移
		enemy_->ChangeState(std::make_unique<EnemyChaseState>());
	}
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

	// 進行方向への速度を設定
	float sign = DirectionToSign(enemy_->GetCurrentDirection());
	enemy_->SetVelocity({ sign * moveSpeed_ * (enemy_->GetPlayer()->IsGoaled() ? 0.0f : 1.0f), 0.0f, 0.0f });

	// 向きの更新
	enemy_->UpdateFacing(sign);
}
 
///-------------------------------------------/// 
/// 状態遷移の確認
///-------------------------------------------///
bool EnemyMoveState::CheckTransition() {
	// 視界の範囲内にPlayerがいるかどうかを確認。
	if (!enemy_ || !enemy_->GetPlayer()) return false;

	// EnemyとPlayerのワールド座標を取得
	Norm::Vector3 enemyPos = enemy_->GetWorldTransform().GetWorldTranslate();
	Norm::Vector3 playerPos = enemy_->GetPlayer()->GetTranslate();

	// Enemyから見たPlayerの相対座標
	float diffX = playerPos.x - enemyPos.x;
	float diffY = playerPos.y - enemyPos.y;

	// 現在Enemyが向いている方向
	float facingSign = DirectionToSign(enemy_->GetCurrentDirection());

	// 向いている方向を基準とした奥行き方向の距離(正の値なら前方)
	float forwardDistance = diffX * facingSign;

	// 扇形範囲内にPlayerがいるかどうか
	bool isPlayerInFanRange =
		(forwardDistance >= 0.0f && forwardDistance <= viewRange_.x) &&
		(std::fabs(diffY) <= viewRange_.y);

	return isPlayerInFanRange;
}
