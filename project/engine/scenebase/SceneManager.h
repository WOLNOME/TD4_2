#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"
#include "SceneTransitionAnimation.h"
#include "Audio.h"
#include <string>
#include <memory>

namespace Norm {

	/// <summary>
	/// シーン遷移のモード
	/// </summary>
	enum class TransitionMode {
		Normal,			//通常遷移
		Temporary,		//仮遷移（Keep）
		FromKeep		//Keepから復帰
	};

	/// <summary>
	/// 全てのシーン処理を行うクラス
	/// シングルトンパターンで実装
	/// </summary>
	class SceneManager {
	private://コンストラクタ等の隠蔽
		static std::unique_ptr<SceneManager> instance_;

		SceneManager() = default;//コンストラクタ隠蔽
		~SceneManager() = default;//デストラクタ隠蔽
		SceneManager(SceneManager&) = delete;//コピーコンストラクタ封印
		SceneManager& operator=(SceneManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<SceneManager>;

	public:
		/// ============================== ///
		///		メンバ関数
		///	============================== ///

		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		/// <returns></returns>
		static SceneManager* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// デバッグ処理
		/// </summary>
		void DebugWithImGui();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// 次のシーンのセット
		/// </summary>
		bool SetNextScene(const std::string& nextSceneName, SceneTransitionAnimation::Type inType = SceneTransitionAnimation::Type::FADE, SceneTransitionAnimation::Type outType = SceneTransitionAnimation::Type::FADE, SceneTransitionAnimation::Option option = SceneTransitionAnimation::Option::NONE, float time = 1.0f, uint32_t _textureHandle = 0u, TransitionMode _transitionMode = TransitionMode::Normal);

		/// ============================== ///
		///		getter
		///	============================== ///

		/// <summary>
		/// 現在のシーンの取得
		/// </summary>
		/// <returns>現在のシーン</returns>
		BaseScene* GetCurrentScene() const { return scene_.get(); }

	private:
		/// ============================== ///
		///		非公開メンバ関数
		///	============================== ///

		/// <summary>
		/// シーン切り替え
		/// </summary>
		void ChangeScene();

		/// ============================== ///
		///		メンバ変数
		///	============================== ///

		//今のシーン
		std::unique_ptr<BaseScene> scene_ = nullptr;
		//次のシーン
		std::unique_ptr<BaseScene> nextScene_ = nullptr;
		//キープシーン
		std::unique_ptr<BaseScene> keepScene_ = nullptr;
		//シーン遷移のモード
		TransitionMode transitionMode_ = TransitionMode::Normal;

		//シーンファクトリー
		std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;
		//シーン遷移アニメーション
		std::unique_ptr<SceneTransitionAnimation> sceneTransitionAnimation_ = nullptr;

	};

}