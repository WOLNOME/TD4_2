#include "EnemyKnockbackState.h"
// Enemy
#include "application/object/entity/enemy/BaseEnemy.h"
// Player
#include "application/object/Character/Player.h"
// Math
#include "MyMath.h"

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
EnemyKnockbackState::EnemyKnockbackState(std::unique_ptr<EnemyState> state) {
	// 所有権を受け取る
	previousState_ = std::move(state);
}

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void EnemyKnockbackState::Enter(BaseEnemy* enemy) {
	// 所有者のポインタをセット
	enemy_ = enemy;

	// ノックバック処理を実行
	KnockBack();
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void EnemyKnockbackState::Update() {

	// ノックバックの減衰処理
	knockbackVelocity_ *= knockbackDamping_;
	enemy_->SetVelocity(knockbackVelocity_);

	/// ===Stateの変更=== ///
	if (std::abs(enemy_->GetVelocity().x) <= 0.01f && std::abs(enemy_->GetVelocity().y) <= 0.01f) {
		enemy_->ChangeState(std::move(previousState_));
	}
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void EnemyKnockbackState::Exit() {
	EnemyState::Exit();
}

///-------------------------------------------/// 
/// ノックバック処理
///-------------------------------------------///
void EnemyKnockbackState::KnockBack() {
	// Playerの方向を求める
	Norm::Vector3 playerDirection = enemy_->GetPlayer()->GetTranslate() - enemy_->GetWorldTransform().GetWorldTranslate();
	// Z軸を無視
	playerDirection.z = 0.0f;

	// 正規化してPlayerとは反対方向にする
	knockbackDirection_ = -playerDirection.Normalize();

	// ノックバックの速度を設定
	knockbackVelocity_ = knockbackDirection_ * KnockbackSpeed_;
	enemy_->SetVelocity(knockbackVelocity_);
}
