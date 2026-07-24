#include "EnemyManager.h"
// c++
#include <algorithm> 
// ImGui
#include <ImGuiManager.h>
// Math
#include <MyMath.h>

///-------------------------------------------/// 
/// デストラクタ
///-------------------------------------------///
EnemyManager::~EnemyManager() {
	ClearEnemies();
}

///-------------------------------------------/// 
/// エネミーの(再)生成
///-------------------------------------------///
void EnemyManager::RespawnEnemy(EnemySlot& slot) {
	// BaseEnemyのインスタンスを生成し、unique_ptrで管理
	slot.enemy = std::make_unique<BaseEnemy>();

	// 初期化メソッドを呼び出す(スポーン位置・初期方向はスロットに保存済み)
	slot.enemy->Initialize(slot.spawnPosition, player_, slot.firstDirection);
	slot.enemy->SetLightManager(lightManager_);

	// リスポーン待機状態を解除
	slot.isRespawning = false;
	slot.respawnTimer = 0.0f;
}

///-------------------------------------------/// 
/// エネミーのスポーン
///-------------------------------------------///
void EnemyManager::SpawnEnemy(
	const Norm::Vector3& position, EnemyDirection firstDirection) {

	// スポーン情報を保持する新しいスロットを作成
	EnemySlot slot;
	slot.spawnPosition = position;
	slot.firstDirection = firstDirection;

	// Enemyを生成
	RespawnEnemy(slot);

	// mapに追加(キーは一意なIDを発行)
	enemies_.emplace(nextEnemyId_++, std::move(slot));
}

///-------------------------------------------/// 
/// エネミーの更新
///-------------------------------------------///
void EnemyManager::UpdateEnemies() {
	for (auto& [id, slot] : enemies_) {
		if (slot.enemy) {
			// Enemyの更新処理
			slot.enemy->Update();

			// 死亡したかどうかを確認
			if (slot.enemy->IsDead()) {
				// 死亡カウントを増加
				deadEnemyCount_++;
				// Enemy本体を解放し、リスポーン待機状態にする
				slot.enemy.reset();
				slot.isRespawning = true;
				slot.respawnTimer = respawnDelayFrames_;
			}
		} else if (slot.isRespawning) {
			// リスポーン待機中の処理
			slot.respawnTimer -= Norm::kDeltaTime;
			if (slot.respawnTimer <= 0.0f) {
				// 待機時間が経過したらスポーン位置に復活させる
				RespawnEnemy(slot);
			}
		}
	}
}

///-------------------------------------------/// 
/// 生存している敵の数を取得する
///-------------------------------------------///
int EnemyManager::GetEnemyCount() const {
	int count = 0;
	for (const auto& [id, slot] : enemies_) {
		if (slot.enemy) {
			++count;
		}
	}
	return count;
}

///-------------------------------------------/// 
/// デバッグ情報
///-------------------------------------------///
void EnemyManager::DebugWithImGui() {
#ifdef _DEBUG

	ImGui::Begin("EnemyManager Debug");
	ImGui::Text("Alive Enemies: %d", GetEnemyCount());
	ImGui::Text("Dead Enemies: %d", GetDeadEnemyCount());
	ImGui::Text("Slots (alive + respawning): %d", static_cast<int>(enemies_.size()));

	// リスポーン待機中の敵の残り時間を表示
	for (const auto& [id, slot] : enemies_) {
		if (slot.isRespawning) {
			ImGui::Text("Enemy[%d] respawn in: %.1f frames", id, slot.respawnTimer);
		}
	}
	ImGui::End();

#endif // _DEBUG
}

///-------------------------------------------/// 
/// エネミーのクリア
///-------------------------------------------///
void EnemyManager::ClearEnemies() {
	enemies_.clear();
}