#pragma once

// Engine
#include <BaseScene.h>
#include <DevelopCamera.h>
#include <Sprite.h>
#include <Audio.h>

// Application
#include <application/object/environment/Skydome.h>

namespace Norm {
// =========================================================
// リザルトシーンクラス
// =========================================================
class GameOverScene : public BaseScene {
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

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// 開発用カメラ
	std::unique_ptr<DevelopCamera> camera_ = nullptr;
	Vector3 cameraTranslate_ = {15.0f, -10.0f, -100.0f};
	Vector3 cameraRotate_ = {0.0f, 0.0f, 0.0f};

	// 平行光源
	std::unique_ptr<DirectionalLight> dirLight_;

	// 天球
	std::unique_ptr<Skydome> skydome_ = nullptr;

	// スプライト
	uint32_t texResult_ = 0u;
	std::unique_ptr<Sprite> spriteResult_ = nullptr;

	// クリック音
	std::unique_ptr<Norm::Audio> seClick_;

	// BGM
	std::unique_ptr<Norm::Audio> bgmResult_;
};
}