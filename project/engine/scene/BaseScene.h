#pragma once
//シーンの基底クラス
#include "SceneLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
//オブジェクトクラス
#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "BulletTrail.h"
#include "CombinedParticle.h"
//オブジェクトマネージャークラス
#include "SpriteManager.h"
#include "Object3dManager.h"
#include "LineManager.h"
#include "ParticleManager.h"
#include "CombinedParticleManager.h"
#include "BulletTrailManager.h"
#include "Input.h"
#include "Audio.h"
#include "TextureManager.h"
#include "TextTextureManager.h"
//シーン共通クラス
#include "PostEffectManager.h"
#include "ImGuiManager.h"
//シーン共通ヘッダー
#include <cstdint>
#include <chrono>
#include <memory>

namespace Norm {

	class SceneManager;

	/// <summary>
	/// シーンの基底クラス
	/// </summary>
	class BaseScene {
	public:
		/// ============================== ///
		///		メンバ関数
		///	============================== ///

		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~BaseScene();
		/// <summary>
		/// 初期化
		/// </summary>
		virtual void Initialize();
		/// <summary>
		/// 終了時
		/// </summary>
		virtual void Finalize();
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update();
		/// <summary>
		/// デバッグ処理
		/// </summary>
		virtual void DebugWithImGui() = 0;

		/// <summary>
		/// 復帰時の処理
		/// </summary>
		virtual void OnResume();

		/// <summary>
		/// FPS表示
		/// </summary>
		void ShowFPS();

		/// ============================== ///
		///		getter
		///	============================== ///

		/// <summary>
		/// シーン名の取得
		/// </summary>
		/// <returns>シーン名</returns>
		const std::string& GetSceneName() const { return sceneName_; }


		/// ============================== ///
		///		setter
		///	============================== ///

		/// <summary>
		/// シーン名のセット
		/// </summary>
		/// <param name="sceneName">シーン名</param>
		void SetSceneName(const std::string& sceneName) { sceneName_ = sceneName; }

	protected:
		/// ============================== ///
		///		メンバ変数
		///	============================== ///

		//シーンマネージャー
		SceneManager* sceneManager_ = nullptr;
		//シーンライト
		std::unique_ptr<SceneLight> sceneLight_ = nullptr;

		//シーン名
		std::string sceneName_ = "";

		//fps計測用変数
		std::chrono::steady_clock::time_point lastFrameTime_ = std::chrono::steady_clock::now();
		float fps_ = 0.0f;

	};

}