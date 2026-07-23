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

		//点光源
		std::unique_ptr<PointLight> pointLight_;

		std::unique_ptr<Object3d> backGroundObject_ = nullptr;
		WorldTransform backGroundWT_;

		std::vector<std::unique_ptr<TitleEnemy>> titleEnemies_;

		UI titleUI_;

		UI tutorialUI_;

		UI buttonUI_;

		UI tutorialButtonUI_;

		UI exitUI_;

		Vector2 initSize_;

		// クリック音
	  std::unique_ptr<Norm::Audio> seClick_;

		// タイトルBGM
	  std::unique_ptr<Norm::Audio> bgmTitle_;
    
		Vector2 titlePos_ = { 640.0f,180.0f };

		Vector2 tutorialPos_ = { 640.0f,360.0f };

		Vector2 buttonPos_ = { 640.0f,400.0f };

		Vector2 tutorialButtonPos_ = { 640.0f,550.0f };

		Vector2 exitUIPos_ = { 60.0f,60.0f };

		Vector2 buttonInitSize_;

		float buttonLength_ = 80.0f;

		float buttonSizeRatio_ = 1.2f;

		float tutorialTimer_ = 0.0f;

		float tutorialMaxTime_ = 1.0f;

		int maxEnemy_ = 5;

		float spawnDirection_ = 1.0f;

		float spawnLengthX_ = 45.0f;

		float spawnLengthY_ = 7.5f;

		float startPosY_ = -15.0f;

		bool isSceneChange_ = false;

		bool isTutorial_ = false;

		float sceneChangeTimer_ = 0.0f;

		float sceneChangeMaxTime_ = 1.0f;

		float lightRadiusBefore_ = 20.0f;

		float lightRadiusAfter_ = 15.0f;

		float lightIntensityBefore_ = 10.0f;

		float lightIntensityAfter_ = 0.0f;
	};
}