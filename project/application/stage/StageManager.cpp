#include "StageManager.h"

// Application
#include <application/stage/StageLoader.h>

void Norm::StageManager::LoadStage(const std::string& jsonPath) {
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

				// 文字列から生成する MapChip::Type　を判別
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

				// 座標計算
				Vector3 position = {
					x * data.tileSize, 
					-y * data.tileSize, // マップエディタに合わせて反対方向へ
					0.0f
				};

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