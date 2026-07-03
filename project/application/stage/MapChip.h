#pragma once

// Engine
#include <Object3d.h>
#include <Object3dManager.h>
#include <WorldTransform.h>

namespace Norm {
// =========================================================
// マップチップクラス（個々のブロック）
// =========================================================
class MapChip {
public:
	/// <summary>
	/// チップの種類
	/// </summary>
	enum class Type { 
		None = 0,
		NormalBlock = 1,
	};

	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="type">チップの種類</param>
	/// <param name="position">位置</param>
	void Initialize(Type type, const Vector3& position);

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// チップの種類
	Type type_ = Type::None;
	// オブジェクト
	std::unique_ptr<Object3d> object_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform wt_;
};
}
