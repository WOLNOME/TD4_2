#include "StageLoader.h"

// C++
#include <fstream>
#include <cassert>

// Externals
#include <json.hpp>


Norm::StageData Norm::StageLoader::LoadJson(const std::string& filePath) { 
	//std::ifstream file(filePath);
	//// ファイルが開けなかったら中断
	//assert(file.is_open());

	//// JSON一括読み込み
	//nlohmann::json json;
	//file >> json;

	StageData stageData;

	//// Tiledの出力するJSON構造を解析する
	//stageData.width = json["width"].get<int32_t>();
	//stageData.height = json["height"].get<int32_t>();

	//// レイヤー配列をループで回す
	//for (const auto& jsonLayer : json["layers"]) {
	//	StageData::Layer layer;
	//	layer.name = jsonLayer["name"].get<std::string>();

	//	// 配列を取得
	//	layer.tiles = jsonLayer["data"].get<std::vector<int32_t>>();

	//	stageData.layers.push_back(layer);
	//}

	// テスト用ダミーデータ
	stageData.width = 7;
	stageData.height = 5;
	StageData::Layer dummyLayer;
	dummyLayer.name = "Ground";
	dummyLayer.tiles = {
	    1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 1,
		1, 0, 1, 0, 1, 0, 1,
		1, 0, 0, 0, 0, 0, 1,
		1, 1, 1, 1, 1, 1, 1,
	};
	stageData.layers.push_back(dummyLayer);

	return stageData;
}
