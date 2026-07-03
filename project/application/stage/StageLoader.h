#pragma once

// C++
#include <string>
#include <vector>
#include <cstdint>

namespace Norm {
	/// <summary>
	/// ローダーが解析した結果を格納する構造体
	/// </summary>
	struct StageData {
		int32_t width = 0;	// マップの横のマス数
	    int32_t height = 0; // マップの縦のマス数
	    float tileSize = 2.0f; // タイル1マスのサイズ（3D空間上の大きさ）

		// レイヤーごとのマップチップIDデータ（2次元配列を1次元にしたもの）
		struct Layer {
		    std::string name;
		    std::vector<int32_t> tiles;
		};
	    std::vector<Layer> layers;
	};

// =========================================================
// ステージ読み込み専用クラス
// =========================================================
class StageLoader {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// JSONファイルを読み込んで構造体に変換
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	static StageData LoadJson(const std::string& filePath);
};
}
