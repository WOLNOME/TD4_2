#pragma once

// C++
#include <string>

// Application
#include <application/stage/MapChip.h>

// グローバル前方宣言
class EnemyManager;
class GimmickManager;

namespace Norm {
// Norm空間の前方宣言
class Player;

// =========================================================
// ステージ管理クラス
// =========================================================
class StageManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// ステージ読み込み処理
	/// </summary>
	/// <param name="jsonPath"></param>
	void LoadStage(const std::string& jsonPath, Player* player, EnemyManager* enemyManager, GimmickManager* gimmickManager);

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// 全チップ
	std::vector<std::unique_ptr<MapChip>> chips_;
};
}
