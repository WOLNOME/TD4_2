#include "EnemyBodyCollider.h"
#include <application/object/entity/enemy/BaseEnemy.h>

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
	if (_attribute == Norm::CollisionAttribute::Gimmick) {
		// カラーを赤に変更
		debugLineColor_ = { 1,0,0,1 };
		// フラグを有効化
		enemy_->SetBodyColliding(true);
	}
}
