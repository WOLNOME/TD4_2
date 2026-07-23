#include "EnemyBodyCollider.h"
#include <application/object/entity/enemy/BaseEnemy.h>
// Player
#include "application/object/Character/Player.h"

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
EnemyBodyCollider::EnemyBodyCollider(BaseEnemy* enemy) {
	// 所有者のポインタをセット
	enemy_ = enemy;
}

///-------------------------------------------/// 
/// デバッグ
///-------------------------------------------///
void EnemyBodyCollider::Debug() {
#ifdef _DEBUG
	// 基底クラスのデバッグ処理
	OBBColliderBase::Debug();

	// カラーを戻す
	debugLineColor_ = { 1,1,1,1 };
#endif // _DEBUG
}

///-------------------------------------------/// 
/// 衝突時のコールバック
///-------------------------------------------///
void EnemyBodyCollider::OnCollision(Norm::ICollider * _other, Norm::CollisionAttribute _attribute) {
	/// ===Gimmick=== ///
	if (_attribute == Norm::CollisionAttribute::Gimmick) {
		// カラーを赤に変更
		debugLineColor_ = { 1,0,0,1 };
		// フラグを有効化
		enemy_->SetBodyColliding(true);
	}

	/// ===Player=== ///
	if (_attribute == Norm::CollisionAttribute::Player) {
		if (!enemy_->GetPlayer()->IsInvincible()) {
			
		}

		// カラーを赤に変更
		debugLineColor_ = { 1,0,0,1 };

		// ノックバック処理を追加
		enemy_->SetPlayerColliding(true);
	}
}
