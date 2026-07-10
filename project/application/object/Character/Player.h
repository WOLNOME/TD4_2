#pragma once

// Engine
#include <Object3d.h>
#include <WorldTransform.h>
#include <Input.h>
#include <ICollider.h>

namespace Norm {
// =========================================================
// プレイヤークラス
// =========================================================
class Player {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

	/// <summary>
	/// 衝突時コールバック
	/// </summary>
	/// <param name="other"></param>
	/// <param name="otherAttr"></param>
	void OnCollision(ICollider* other, CollisionAttribute otherAttr);

	// =========================================================
	// Accessor
	// =========================================================

	/// <summary>
	/// 位置の取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetTranslate() const { return wt_.GetTranslate(); }

private:
	// =========================================================
	// Constants
	// =========================================================

	// 移動速度
	const float kSpeed = 0.3f;
	// 重力
	const float kGravity = -0.02f;
	// ジャンプの初速
	const float kJumpPower = 0.4f;

	// =========================================================
	// Member Variables
	// =========================================================

	// 入力システム
	Input* input_ = nullptr;
	// オブジェクト
	std::unique_ptr<Object3d> object_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform wt_;
	// コライダー
	std::unique_ptr<ICollider> collider_ = nullptr;

	// 速度
	Vector3 velocity_ = {0.0f, 0.0f, 0.0f};
	// 垂直方向の速度
	float yVelocity_ = 0.0f;
	// 接地フラグ
	bool isGrounded_ = false;

	// ゴール済みフラグ
	bool isGoaled_ = false;
};
} 
