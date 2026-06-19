#pragma once
#include "OBBColliderBase.h"
#include <cstdint>

//前方宣言（アプリケーション）
class IBaseLevelObject;

/// <summary>
/// レベルオブジェクトのコライダー
/// </summary>
class LevelObjectCollider : public Norm::OBBColliderBase {
public:
	/// ============================== ///
	///		メンバ関数
	/// ============================== ///

	/// <summary>
	/// コンストラクタ
	/// </summary>
	LevelObjectCollider(IBaseLevelObject* _holder);
	/// <summary>
	/// デストラクタ
	/// </summary>
	~LevelObjectCollider() override = default;

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug() override;

	/// <summary>
	/// 衝突時のコールバック
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnCollision(Norm::ICollider* _other, Norm::CollisionAttribute _attribute) override;

	/// ============================== ///
	///		setter
	/// ============================== ///

	/// <summary>
	/// ハンドルのセット
	/// </summary>
	/// <param name="_handle"></param>
	void SetHandle(uint32_t _handle) { handle_ = _handle; }


private:
	//所有者のポインタ
	IBaseLevelObject* holder_ = nullptr;

	//オブジェクトのハンドル
	uint32_t handle_ = 0u;

};

