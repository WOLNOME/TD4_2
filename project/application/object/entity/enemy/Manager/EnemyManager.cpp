#include "EnemyManager.h"
// c++
#include <algorithm> 
// ImGui
#include <ImGuiManager.h>

///-------------------------------------------/// 
/// エネミーのスポーン
///-------------------------------------------///
void EnemyManager::SpawnEnemy(
	const Norm::Vector3& position, EnemyDirection firstDirection) {

	// BaseEnemyのインスタンスを生成し、unique_ptrで管理
	std::unique_ptr<BaseEnemy> enemy = std::make_unique<BaseEnemy>();

	// 初期化メソッドを呼び出す
	enemy->Initialize(position, player_, firstDirection);
    enemy->SetLightManager(lightManager_);

	// 配列に追加
	enemies_.push_back(std::move(enemy));
}

///-------------------------------------------/// 
/// エネミーの更新
///-------------------------------------------///
void EnemyManager::UpdateEnemies() {
	// Enemyの更新処理
    for (auto& enemy : enemies_) {
        if (enemy) {
            enemy->Update();
        }
    }

    // 死亡した敵をリストから削除
    for (auto it = enemies_.begin(); it != enemies_.end(); ) {
        if ((*it)->IsDead()) {
            // 死亡カウントを増加
            deadEnemyCount_++;            
            it = enemies_.erase(it);
        } else {
			// 次の要素に進む
            ++it;
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
