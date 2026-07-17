#include "ResultScene.h"

// Engine
#include <Input.h>
#include <SceneManager.h>

void Norm::ResultScene::Initialize() {
	/* シーン共通初期化処理 */
	BaseScene::Initialize();

	/* 開発用カメラ生成 + 初期化 */
	camera_ = std::make_unique<DevelopCamera>();
	camera_->Initialize();
	camera_->SetFarClip(1000.0f);
	camera_->worldTransform.SetTranslate(cameraTranslate_);
	camera_->worldTransform.SetRotate(cameraRotate_);
	// 開発用カメラをセット
	Object3dManager::GetInstance()->SetCamera(camera_.get());
	LineManager::GetInstance()->SetCamera(camera_.get());
	ParticleManager::GetInstance()->SetCamera(camera_.get());

	/* ライト生成 + 初期化 */
	dirLight_ = std::make_unique<DirectionalLight>();
	dirLight_->SetIntensity(0.05f);
	dirLight_->SetColor({1, 0, 0, 1});
	dirLight_->SetDirection({0.5f, 0.5f, -1.0f});
	// ライトを登録
	sceneLight_->SetLight(dirLight_.get());

	/* 天球の生成 + 初期化 */
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();
}

void Norm::ResultScene::Finalize() {}

void Norm::ResultScene::Update() {
	/* シーン共通更新処理 */
	BaseScene::Update();
	/* カメラ更新処理 */
	camera_->Update();

	// スペースキーでタイトルへ（仮）
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		sceneManager_->SetNextScene("TITLE");
	}
}

void Norm::ResultScene::DebugWithImGui() {}
