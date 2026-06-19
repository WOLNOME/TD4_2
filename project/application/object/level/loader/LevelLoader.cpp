#include "LevelLoader.h"
#include <assert.h>

using namespace Norm;

void LevelLoader::Initialize(const std::string& _filePath) {
	//jsonデータを読み込む
	json data = JsonUtil::GetJsonData(_filePath);

	//各オブジェクトの生成
	

	//各オブジェクトの初期化
	

	//全オブジェクトデータのロード
	for (json& object : data["objects"]) {
		ScanObjectData(object);
	}
}

void LevelLoader::Update() {
	//オブジェクトの更新
	
}

void LevelLoader::DebugWithImGui() {
#ifdef _DEBUG
	//全オブジェクトのデバッグ処理
	
#endif // _DEBUG

}

void LevelLoader::ScanObjectData(json& object) {
	//前回のオブジェクトが残ってしまっていたので、一旦処理を削除
	object;
}
