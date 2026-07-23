#include "GameOverScene.h"

// Engine
#include <Input.h>
#include <SceneManager.h>

void Norm::GameOverScene::Initialize() {
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

	/* スプライト生成 + 初期化 */
	spriteResult_ = std::make_unique<Sprite>();
	texResult_ = TextureManager::GetInstance()->LoadTexture("gameover.png");
	spriteResult_->Initialize(SpriteTag{}, Object3dManager::GetInstance()->GenerateName("Sprite"), Order::Front0, texResult_);

	// 音声読み込み
	seClick_ = std::make_unique<Norm::Audio>();
	seClick_->Initialize("click.wav");

	bgmResult_ = std::make_unique<Norm::Audio>();
	bgmResult_->Initialize("resultBGM.wav");
	bgmResult_->Play(true, 0.5f);

	PostEffectManager::GetInstance()->AddPostEffectOrder(PostEffectKind::None);
}

void Norm::GameOverScene::Finalize() {}

void Norm::GameOverScene::Update() {
	/* シーン共通更新処理 */
	BaseScene::Update();
	/* カメラ更新処理 */
	camera_->Update();

	// 左クリックでタイトルへ（仮）
	if (Input::GetInstance()->TriggerMouseButton(MouseButton::LeftButton)) {
		// クリック音再生
		seClick_->Play(false, 0.5f);

		sceneManager_->SetNextScene("TITLE");
	}
}

void Norm::GameOverScene::DebugWithImGui() {}
