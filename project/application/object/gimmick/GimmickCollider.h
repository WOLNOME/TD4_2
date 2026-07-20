#pragma once
#include "OBBColliderBase.h"

using namespace Norm;

class GimmickBase;

class GimmickCollider :
	public OBBColliderBase
{
public:
	/// <summary>
	///コンフリクトラクタ
	/// </summary>
	GimmickCollider(GimmickBase* _holder);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GimmickCollider() override = default;

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
	GimmickBase* holder_ = nullptr;



};

