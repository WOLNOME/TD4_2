#pragma once
/// ===Include=== ///
#include "OBBColliderBase.h"
#include <memory>

/// ===前方宣言=== ///
class BaseEnemy;

///=====================================================/// 
/// EnemyAreaCollider
/// Enemyの行動範囲用コライダー
///=====================================================///
class EnemyAreaCollider : public Norm::OBBColliderBase {
public:
	///-------------------------------------------/// 
	/// メンバ関数
	///-------------------------------------------///

	EnemyAreaCollider(BaseEnemy* enemy);
	~EnemyAreaCollider() override = default;

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug() override;

	/// <summary>
	/// 衝突時のコールバック
	/// </summary>
	/// <param name="_other">衝突したコライダー</param>
	/// <param name="_attribute">衝突したコライダーの属性</param>
	void OnCollision(Norm::ICollider* _other, Norm::CollisionAttribute _attribute) override;

private:
	// 所有者のポインタ
	BaseEnemy* enemy_ = nullptr;
};