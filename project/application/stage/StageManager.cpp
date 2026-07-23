#include "StageManager.h"

// Application
#include <application/stage/StageLoader.h>
#include <application/object/Character/Player.h>
#include <application/object/entity/enemy/Manager/EnemyManager.h>
#include <application/object/gimmick/GimmickManager.h>

void Norm::StageManager::LoadStage(const std::string& jsonPath, Player* player, EnemyManager* enemyManager, GimmickManager* gimmickManager) {
	chips_.clear();

	// ローダーを使ってデータを取得
	StageData data = StageLoader::LoadJson(jsonPath);

	// 取得したデータを元に、ループしてオブジェクトを生成
	for (const auto& layer : data.layers) {
		for (int32_t y = 0; y < data.height; ++y) {
			for (int32_t x = 0; x < data.width; ++x) {
				// 1次元配列からインデックスを計算
				int32_t index = y * data.width + x;
				int32_t chipID = layer.tiles[index]; // gid

				if (chipID == 0) {
					continue; // 空白ならスキップ
				}

				// Gidから本来のタイルIDを割り出し、BlockType文字列を取得
				int32_t localTileId = chipID - data.firstGid;
				std::string blockTypeStr = "None";

				if (data.tileTypes.count(localTileId) > 0) {
					blockTypeStr = data.tileTypes[localTileId];
				}

				// 座標計算
				Vector3 position = {
				    x * data.tileSize,
				    -y * data.tileSize, // マップエディタに合わせて反対方向へ
				    0.0f};

				// プレイヤーの配置
				if (blockTypeStr == "player") {
					if (player) {
						player->SetTranslate(position);
					}
					continue;
				}

				// 敵の生成
				if (blockTypeStr == "ghost") {
					if (enemyManager) {
						enemyManager->SpawnEnemy(position);
					}
					continue;
				}

				// 爆弾の生成
				if (blockTypeStr == "bomb") {
					if (gimmickManager) {
						gimmickManager->CreateGimmick(GimmickType::Explosion, position);
					}
					continue;
				}

				// 各ブロックの生成
				MapChip::Type type = MapChip::Type::None;
				if (blockTypeStr == "normalBlock") {
					type = MapChip::Type::NormalBlock;
				} else if (blockTypeStr == "areaBlock") {
					type = MapChip::Type::AreaBlock;
				} else if (blockTypeStr == "goalBlock") {
					type = MapChip::Type::GoalBlock;
				}

				// 該当するタイプが無い、またはNoneなら生成スキップ
				if (type == MapChip::Type::None) {
					continue;
				}

				// チップの生成・登録
				auto chip = std::make_unique<MapChip>();
				chip->Initialize(type, position);
				chips_.push_back(std::move(chip));
			}
		}
	}
}

void Norm::StageManager::Debug() {
#ifdef _DEBUG
	for (auto& chip : chips_) {
		chip->Debug();
	}
#endif
}