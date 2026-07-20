#include "EnemyAreaCollider.h"
#include <application/object/entity/enemy/BaseEnemy.h>

///-------------------------------------------/// 
/// コンストラクタ
///-------------------------------------------///
EnemyAreaCollider::EnemyAreaCollider(BaseEnemy* enemy) {
	// 所有者のポインタをセット
	enemy_ = enemy;
}

///-------------------------------------------/// 
/// デバッグ
///-------------------------------------------///
void EnemyAreaCollider::Debug() {
#ifdef _DEBUG
	// 基底クラスのデバッグ処理
	OBBColliderBase::Debug();

	// カラーを戻す
	debugLineColor_ = { 0,0,1,1 };
#endif // _DEBUG
}

///-------------------------------------------/// 
/// 衝突時のコールバック
///-------------------------------------------///
void EnemyAreaCollider::OnCollision(Norm::ICollider* _other, Norm::CollisionAttribute _attribute) {
	
	// 衝突時の処理
	if (_attribute == Norm::CollisionAttribute::Area) {
		// カラーを赤に変更
		debugLineColor_ = { 1,0,0,1 };

		// フラグを有効化
		enemy_->SetAreaColliding(true);

		// お互いのコライダーを取得
		auto* enemyCollider = dynamic_cast<OBBColliderBase*>(this);
		auto* areaCollider = dynamic_cast<OBBColliderBase*>(_other);

		if (enemyCollider && areaCollider) {
			Norm::OBB enemyOBB = enemyCollider->GetOBB();
			Norm::OBB areaOBB = areaCollider->GetOBB();

			Norm::Vector3 pushVector = { 0.0f, 0.0f, 0.0f };

			if (Norm::MyMath::CalculatePushVector(enemyOBB, areaOBB, &pushVector)) {
				// 敵の座標を押し戻しベクトル分だけ戻す
				Norm::Vector3 currentPos = enemy_->GetWorldTransform().GetTranslate();
				enemy_->GetWorldTransform().SetTranslate(Norm::MyMath::Add(currentPos, pushVector));
				enemy_->GetWorldTransform().UpdateMatrix();
			}
		}
	}
}