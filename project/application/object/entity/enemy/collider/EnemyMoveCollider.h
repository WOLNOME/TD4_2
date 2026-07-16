#pragma once
/// ===Include=== ///
#include "OBBColliderBase.h"
#include <memory>

class BaseEnemy;

///-------------------------------------------/// 
/// EnemyMoveCollider
/// エネミーの足用コライダー
///-------------------------------------------///
class EnemyMoveCollider : public Norm::OBBColliderBase {
public:
	/// ============================== ///
	///		メンバ関数
	/// ============================== ///
	
	EnemyMoveCollider(BaseEnemy* enemy);
	~EnemyMoveCollider() override = default;

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