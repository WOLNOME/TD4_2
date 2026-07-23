#pragma once
/// ===Include=== ///
#include <application/object/entity/enemy/BaseEnemy.h>
#include <memory>
#include <unordered_map>

/// ===前方宣言=== ///
class LightManager;
namespace Norm {
	class Player;
}

///=====================================================/// 
/// EnemySlot
/// 1体のEnemy枠を表す。Enemy本体・スポーン情報・リスポーン状態を保持する
///=====================================================///
struct EnemySlot {
	// Enemy本体(死亡中はnullptr)
	std::unique_ptr<BaseEnemy> enemy = nullptr;
	// スポーン位置
	Norm::Vector3 spawnPosition = { 0.0f, 0.0f, 0.0f };
	// 初期方向
	EnemyDirection firstDirection = EnemyDirection::Left;
	// リスポーン待機中かどうか
	bool isRespawning = false;
	// リスポーンまでの残りフレーム数
	float respawnTimer = 0.0f;
};

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
	/// 生存している敵の数を取得する
	/// </summary>
	/// <returns></returns>
	int GetEnemyCount() const;

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

	/// <summary>
	/// リスポーンまでの待機フレーム数を設定する
	/// </summary>
	/// <param name="frames"></param>
	void SetRespawnDelayFrames(float frames) { respawnDelayFrames_ = frames; }

private:
	/// ============================== ///
	///		メンバ関数
	/// ============================== ///

	/// <summary>
	/// 指定したスロットにEnemyを(再)生成する
	/// </summary>
	/// <param name="slot"></param>
	void RespawnEnemy(EnemySlot& slot);

private:
	/// ============================== ///
	///		メンバ変数
	/// ============================== ///
	// 敵を管理するmap (キー: 一意のEnemy ID)
	std::unordered_map<int, EnemySlot> enemies_ = {};

	// 次に発行するEnemy ID
	int nextEnemyId_ = 0;

	// プレイヤーのポインタ
	Norm::Player* player_ = nullptr;

	// LightManagerのポインタ
	LightManager* lightManager_ = nullptr;

	// 死亡した敵の数
	int deadEnemyCount_ = 0;

	// リスポーンまでの時間
	float respawnDelayFrames_ = 20.0f;
};