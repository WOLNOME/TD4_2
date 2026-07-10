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
				int32_t chipID = layer.tiles[index];

				if (chipID == 0)
					continue; // 空白ならスキップ

				// 座標計算
				Vector3 position = {
				    x * data.tileSize,
				    -y * data.tileSize, // エディタと合わせるためY軸を反対に
				    0.0f};

				// チップの生成・登録
				auto chip = std::make_unique<MapChip>();
				chip->Initialize(static_cast<MapChip::Type>(chipID), position);
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