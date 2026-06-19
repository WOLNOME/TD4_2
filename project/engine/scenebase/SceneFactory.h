#pragma once
#include <memory>
#include "AbstractSceneFactory.h"

namespace Norm {

	/// <summary>
	/// 文字列を入力してシーンを生成するためのクラス
	/// </summary>
	class SceneFactory : public AbstractSceneFactory {
	public:
		/// <summary>
		/// シーン生成
		/// </summary>
		/// <param name="sceneName">シーン名</param>
		/// <returns>生成したシーン</returns>
		std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) override;

	};

}
