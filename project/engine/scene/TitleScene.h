#pragma once

// Engine
#include <BaseScene.h>
#include <DevelopCamera.h>
#include <Object3d.h>
#include <WorldTransform.h>

// Application
#include "application/object/entity/titleEnemy/TitleEnemy.h"

#include <memory>

namespace Norm {
	// =========================================================
	// ゲームプレイシーンクラス
	// =========================================================
	class TitleScene : public BaseScene {

	private:

		struct UI {
			int32_t textureHandle = EOF;
			std::unique_ptr<Sprite> sprite = nullptr;
		};

	public:
		// =========================================================
		// Public Methods
		// =========================================================

		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize() override;

		/// <summary>
		/// 終了時処理
		/// </summary>
		void Finalize() override;

		/// <summary>
		/// 更新処理
		/// </summary>
		void Update() override;

		/// <summary>
		/// デバッグ処理
		/// </summary>
		void DebugWithImGui() override;

		/// <summary>
		/// 点光源（ライト）の操作処理
		/// </summary>
		void LightMoveProcess();

	private:
		// =========================================================
		// Member Variables
		// =========================================================

		// 開発用カメラ
		std::unique_ptr<DevelopCamera> camera_ = nullptr;
		Vector3 cameraTranslate_ = { 0.0f, 0.0f, -100.0f };
		Vector3 cameraRotate_ = { 0.0f, 0.0f, 0.0f };

		// 平行光源
		std::unique_ptr<DirectionalLight> dirLight_;

		//点光源
		std::unique_ptr<PointLight> pointLight_;

		std::unique_ptr<Object3d> backGroundObject_ = nullptr;
		WorldTransform wt_;

		std::vector<std::unique_ptr<TitleEnemy>> titleEnemies_;

		UI titleUI_;

		UI buttonUI_;

		Vector2 initSize_;

		int maxEnemy_ = 5;

		bool isSceneChange_ = false;

		float sceneChangeTimer_ = 0.0f;

		float sceneChangeMaxTime_ = 2.0f;

	};
}