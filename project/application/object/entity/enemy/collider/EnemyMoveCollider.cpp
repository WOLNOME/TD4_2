#include "EnemyMoveCollider.h"
#include <application/object/entity/enemy/BaseEnemy.h>

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
EnemyMoveCollider::EnemyMoveCollider(BaseEnemy* enemy) {
	// 所有者のポインタをセット
	enemy_ = enemy;
}

///-------------------------------------------/// 
/// デバッグ
///-------------------------------------------///
void EnemyMoveCollider::Debug() {
#ifdef _DEBUG
	// 基底クラスのデバッグ処理
	OBBColliderBase::Debug();

	// カラーを戻す
	debugLineColor_ = { 0,1,0,1 };
#endif // _DEBUG
}

///-------------------------------------------/// 
/// 衝突時のコールバック
///-------------------------------------------///
void EnemyMoveCollider::OnCollision(Norm::ICollider * _other, Norm::CollisionAttribute _attribute) {
	
	// 衝突時の処理
	if (_attribute == Norm::CollisionAttribute::Block || _attribute == Norm::CollisionAttribute::Area) {

		// カラーを赤に変更
		debugLineColor_ = { 1,0,0,1 };
		// フラグを有効化
		enemy_->SetFootColliding(true);
	}
}
