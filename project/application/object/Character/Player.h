#pragma once

// Engine
#include <Object3d.h>
#include <WorldTransform.h>
#include <Input.h>

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
	// Member Variables
	// =========================================================

	// 入力システム
	Input* input_ = nullptr;
	// オブジェクト
	std::unique_ptr<Object3d> object_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform wt_;

	// 速度
	Vector3 velocity_ = {0.0f, 0.0f, 0.0f};
	// 移動速度
	float speed_ = 0.2f;
};
} 
