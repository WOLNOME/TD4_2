#include "SceneFactory.h"
#include "ParticleEditorScene.h"
#include "SampleScene.h"
#include "GamePlayScene.h"

//アプリケーション

namespace Norm {

	std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName) {
		//次のシーンを生成
		std::unique_ptr<BaseScene> newScene = nullptr;

		// パーティクルエディター
		if (sceneName == "PARTICLEEDITOR") {
			newScene = std::make_unique<ParticleEditorScene>();
		}
		else if (sceneName == "Sample") {
			//サンプルシーン
			newScene = std::make_unique<SampleScene>();
	    } else if (sceneName == "GAMEPLAY") {
			// ゲームプレイシーン
		    newScene = std::make_unique<GamePlayScene>();
		}

		//シーン名をセット
		newScene->SetSceneName(sceneName);

		return newScene;
	}

}