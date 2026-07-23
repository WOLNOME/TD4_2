#include "SceneFactory.h"
#include "ParticleEditorScene.h"
#include "SampleScene.h"
#include "GamePlayScene.h"
#include "TitleScene.h"
#include "ResultScene.h"
#include "GameOverScene.h"

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
		} else if (sceneName == "TITLE") {
			// タイトルシーン
			newScene = std::make_unique<TitleScene>();
	    } else if (sceneName == "RESULT") {
			// リザルトシーン
		    newScene = std::make_unique<ResultScene>();
		} else if (sceneName == "GAMEOVER") {
			// リザルトシーン
			newScene = std::make_unique<GameOverScene>();
		}

		//シーン名をセット
		newScene->SetSceneName(sceneName);

		return newScene;
	}

}