#include "EnemyManager.h"
// c++
#include <algorithm> 
// ImGui
#include <ImGuiManager.h>

///-------------------------------------------/// 
/// エネミーのスポーン
///-------------------------------------------///
void EnemyManager::SpawnEnemy(
	const Norm::Vector3& position, Norm::Player* player, EnemyDirection firstDirection) {

	// BaseEnemyのインスタンスを生成し、unique_ptrで管理
	std::unique_ptr<BaseEnemy> enemy = std::make_unique<BaseEnemy>();

	// 初期化メソッドを呼び出す
	enemy->Initialize(position, player, firstDirection);

	// 配列に追加
	enemies_.push_back(std::move(enemy));
}

///-------------------------------------------/// 
/// エネミーの更新
///-------------------------------------------///
void EnemyManager::UpdateEnemies() {
	for (auto& enemy : enemies_) {
		if (enemy) {
			// 死亡しているかどうかを確認
			if (enemy->IsDead()) {
				deadEnemyCount_++;
			}

			// エネミーの更新処理
			enemy->Update();
		}
	}
}

///-------------------------------------------/// 
/// デバッグ情報
///-------------------------------------------///
void EnemyManager::DebugWithImGui() {
#ifdef _DEBUG

	ImGui::Begin("EnemyManager Debug");
	ImGui::Text("Total Enemies: %d", GetEnemyCount());
	ImGui::Text("Dead Enemies: %d", GetDeadEnemyCount());
	ImGui::End();

#endif // _DEBUG
}

///-------------------------------------------/// 
/// エネミーのクリア
///-------------------------------------------///
void EnemyManager::ClearEnemies() {
	enemies_.clear();
}
