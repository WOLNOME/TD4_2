#pragma once
/// ===Include=== ///
#include <OBBColliderBase.h>
#include <memory>

/// ===前方宣言=== ///
class BaseEnemy;

///=====================================================/// 
/// EnemyBodyCollider
/// Enemyの体用コライダーs
///=====================================================///
class EnemyBodyCollider : public Norm::OBBColliderBase {
public:
	///-------------------------------------------/// 
	/// メンバ関数
	///-------------------------------------------///

	EnemyBodyCollider(BaseEnemy* enemy);
	~EnemyBodyCollider() override = default;

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

