#include "CinematicBuilder.h"
#include "JsonUtil.h"
#include <unordered_map>
#include <cassert>
#include <MyMath.h>

//アプリケーション
#include <application/system/direction/shot/base/IShotBase.h>

//個別ショットの各種ヘッダーファイル

using namespace Norm;

std::vector<std::unique_ptr<IShotBase>> CinematicBuilder::BuildCinematic(const std::string& _fileName, Norm::BlackBoard* _blackBoard) {
	//出力用のデータ構造
	std::vector<std::unique_ptr<IShotBase>> result;

	//JSONファイルを読み込む
	json jsonData;
	jsonData = JsonUtil::GetJsonData(_fileName);

	//jsonで取得した全ての要素を調べる
	for (auto& data : jsonData) {
		//番号を取得
		uint32_t index = data["index"];

		// 必要ならサイズ拡張
		if (result.size() <= index) {
			result.resize(index + 1);
		}

		//カメラトランスフォームを取得
		TransformEuler start = {
			.scale = {1,1,1},
			.rotate = {
				data["startCameraTransform"]["rotate"]["x"],
				data["startCameraTransform"]["rotate"]["y"],
				data["startCameraTransform"]["rotate"]["z"]
			},
			.translate = {
				data["startCameraTransform"]["translate"]["x"],
				data["startCameraTransform"]["translate"]["y"],
				data["startCameraTransform"]["translate"]["z"]
			}
		};
		TransformEuler end = {
			.scale = {1,1,1},
			.rotate = {
				data["endCameraTransform"]["rotate"]["x"],
				data["endCameraTransform"]["rotate"]["y"],
				data["endCameraTransform"]["rotate"]["z"]
			},
			.translate = {
				data["endCameraTransform"]["translate"]["x"],
				data["endCameraTransform"]["translate"]["y"],
				data["endCameraTransform"]["translate"]["z"]
			}
		};
		//尺を取得
		float duration = data["duration"];
		//名前を取得
		std::string name = data["name"];

		//ショットを生成
		std::unique_ptr<IShotBase> shot = nullptr;

		// nameによって個別に生成
		using ShotFactory = std::function<std::unique_ptr<IShotBase>()>;

		std::unordered_map<std::string, ShotFactory> factory = {
			
		};

		auto it = factory.find(name);
		if (it != factory.end()) {
			shot = it->second();
		}
		else {
			assert(false && "該当の名前は存在しません");
		}

		//データを入れる
		result[index] = std::move(shot);
	}

	return result;
}
