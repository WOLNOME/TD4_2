#pragma once

// Engine
#include <BaseScene.h>
#include <DevelopCamera.h>

// Application
#include <application/object/environment/Skydome.h>
#include <application/object/environment/Ground.h>

namespace Norm {

// =========================================================
// ゲームプレイシーンクラス
// =========================================================
class GamePlayScene : public BaseScene {
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
	Vector3 cameraTranslate_ = {0.0f, 0.0f, -50.0f};
	Vector3 cameraRotate_ = {0.0f, 0.0f, 0.0f};
	// 平行光源
	std::unique_ptr<DirectionalLight> dirLight_;
	// 天球
	std::unique_ptr<Skydome> skydome_ = nullptr;

	// テスト用オブジェクト
	std::unique_ptr<Object3d> object_ = nullptr;
	WorldTransform objectWT_;
};
}