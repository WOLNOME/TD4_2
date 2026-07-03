#pragma once
#include "OBBColliderBase.h"
#include "Object3d.h"
#include <memory>

/// <summary>
/// オブジェクトの当たり判定
/// </summary>
class ObjectCollider : public Norm::OBBColliderBase {
public:
	/// ============================== ///
	///		メンバ関数
	/// ============================== ///

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ObjectCollider(Norm::Object3d* _holder);
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ObjectCollider() override = default;

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug() override;

	/// <summary>
	/// 衝突時のコールバック
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnCollision(Norm::ICollider* _other, Norm::CollisionAttribute _attribute) override;

private:
	//所有者のポインタ
	Norm::Object3d* holder_ = nullptr;
};

