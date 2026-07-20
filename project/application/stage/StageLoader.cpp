#include "StageLoader.h"

// C++
#include <fstream>
#include <cassert>

// Externals
#include <json.hpp>


Norm::StageData Norm::StageLoader::LoadJson(const std::string& filePath) { 
	std::ifstream file(filePath);
	// ファイルが開けなかったら中断
	assert(file.is_open());

	// JSON一括読み込み
	nlohmann::json json;
	file >> json;

	StageData stageData;

	// Tiledの出力するJSON構造を解析する
	stageData.width = json["width"].get<int32_t>();
	stageData.height = json["height"].get<int32_t>();

	// タイルセットのカスタムプロパティ解析
	if (json.contains("tilesets") && !json["tilesets"].empty()) {
		const auto& tileset = json["tilesets"][0];
		stageData.firstGid = tileset["firstgid"].get<int32_t>();

		// tiles配列がある場合のみ解析
		if (tileset.contains("tiles")) {
			for (const auto& tileJson : tileset["tiles"]) {
				int32_t tileId = tileJson["id"].get<int32_t>();
				std::string blockTypeStr = "None";

				// properties配列から BlockType を探す
				if (tileJson.contains("properties")) {
					for (const auto& prop : tileJson["properties"]) {
						if (prop["name"].get<std::string>() == "BlockType") {
							blockTypeStr = prop["value"].get<std::string>();
							break;
						}
					}
				}
				// 対応表に登録
				stageData.tileTypes[tileId] = blockTypeStr;
			}
		}
	}

	// レイヤー配列をループで回す
	for (const auto& jsonLayer : json["layers"]) {
		StageData::Layer layer;
		layer.name = jsonLayer["name"].get<std::string>();

		// 配列を取得
		layer.tiles = jsonLayer["data"].get<std::vector<int32_t>>();

		stageData.layers.push_back(layer);
	}

	return stageData;
}
