#pragma once
/// ===Include=== ///
#include <application/object/entity/enemy/BaseEnemy.h>
#include <memory>
#include <vector>

/// ===前方宣言=== ///
class LightManager;
namespace Norm {
	class Player;
}

///=====================================================/// 
/// EnemyManager
/// Enemyの管理クラス
///=====================================================///
class EnemyManager {
public:

	/// <summary>
	/// 敵を生成する
	/// </summary>
	/// <param name="position">スポーン位置</param>
	/// <param name="firstDirection">初期方向</param>
	void SpawnEnemy(const Norm::Vector3& position, EnemyDirection firstDirection = EnemyDirection::Left);

	/// <summary>
	/// 敵を更新する
	/// </summary>
	void UpdateEnemies();

	/// <summary>
	/// ImGuiデバッグ処理
	/// </summary>
	void DebugWithImGui();

	/// <summary>
	/// 敵をクリアする
	/// </summary>
	void ClearEnemies();

public:
	/// ===Getter=== ///

	/// <summary>
	/// 敵の数を取得する
	/// </summary>
	/// <returns></returns>
	int GetEnemyCount() const { return static_cast<int>(enemies_.size()); }

	/// <summary>
	/// 死亡した敵の数を取得する
	/// </summary>
	/// <returns></returns>
	int GetDeadEnemyCount() const { return deadEnemyCount_; }

public:
	/// ===Setter=== ///
	
	/// <summary>
	/// Playerのポインタを設定する
	/// </summary>
	/// <param name="player"></param>
	void SetPlayer(Norm::Player* player) { player_ = player; }

	/// <summary>
	/// LightManagerのポインタを設定する
	/// </summary>
	/// <param name="lightManager"></param>
	void SetLightManager(LightManager* lightManager) { lightManager_ = lightManager; }

private:
	/// ============================== ///
	///		メンバ変数
	/// ============================== ///
	//　敵のポインタを保持する配列
	std::vector<std::unique_ptr<BaseEnemy>> enemies_ = {};

	// プレイヤーのポインタ
	Norm::Player* player_ = nullptr;

	// LightManagerのポインタ
	LightManager* lightManager_ = nullptr;

	// 死亡した敵の数
	int deadEnemyCount_ = 0;
};


