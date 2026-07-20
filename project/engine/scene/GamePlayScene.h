#pragma once

// Engine
#include <BaseScene.h>
#include <DevelopCamera.h>
#include <Object3d.h>

// Application
#include <application/object/entity/enemy/Manager/EnemyManager.h>
#include <application/object/environment/Ground.h>
#include <application/object/environment/Skydome.h>

#include <application/ui/guide/GuideUI.h>

#include <application/object/Character/Player.h>
#include <application/object/camera/FollowCamera.h>
#include <application/stage/StageManager.h>
#include <application/ui/pause/PauseMenu.h>

//#include "ExplosionGimmick.h"
#include "application/object/gimmick/GimmickManager.h"

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

	/// <summary>
	/// 点光源（ライト）の操作処理
	/// </summary>
	void LightMoveProcess();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// カメラ
	std::unique_ptr<FollowCamera> camera_ = nullptr;
	Vector3 cameraTranslate_ = {15.0f, -10.0f, -100.0f};
	Vector3 cameraRotate_ = {0.0f, 0.0f, 0.0f};

	// 平行光源
	std::unique_ptr<DirectionalLight> dirLight_;
	// 点光源
	std::unique_ptr<PointLight> pointLight_;

	// 天球
	std::unique_ptr<Skydome> skydome_ = nullptr;
	// 背景
	std::unique_ptr<Object3d> background_ = nullptr;
	WorldTransform backgroundWT_;

	// ステージ管理クラス
	std::unique_ptr<StageManager> stageManager_ = nullptr;

	// プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

	// Enemy
	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;

	////ギミック
	//std::unique_ptr<ExplosionGimmick> explosionGimmick_ = nullptr;
	//LightInfo lightInfo_{};
	std::unique_ptr<GimmickManager> gimmickManager_;
	LightInfo lightInfo_{};

	// ポーズメニュー
	std::unique_ptr<PauseMenu> pauseMenu_;
	std::unique_ptr<GuideUI> guideUI_ = nullptr;
};
} // namespace Norm