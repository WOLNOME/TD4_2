#include "GamePlayScene.h"

// Engine
#include <CollisionManager.h>
#include <Input.h>
#include <SceneManager.h>
#include <Object3dManager.h>

void Norm::GamePlayScene::Initialize() {
	/* シーン共通初期化処理 */
	BaseScene::Initialize();

	/* プレイヤー生成 + 初期化 */
	player_ = std::make_unique<Player>();
	player_->Initialize();

	/* カメラ生成 + 初期化（プレイヤー生成の後） */
	camera_ = std::make_unique<FollowCamera>();
	camera_->Initialize(player_.get());
	camera_->SetFarClip(1000.0f);
	camera_->worldTransform.SetTranslate(cameraTranslate_);
	camera_->worldTransform.SetRotate(cameraRotate_);
	// カメラをセット
	Object3dManager::GetInstance()->SetCamera(camera_.get());
	LineManager::GetInstance()->SetCamera(camera_.get());
	ParticleManager::GetInstance()->SetCamera(camera_.get());

	/* ライト生成 + 初期化 */
	dirLight_ = std::make_unique<DirectionalLight>();
	dirLight_->SetIntensity(0.01f);
	dirLight_->SetColor({ 1,1,1,1 });
	dirLight_->SetDirection({ 0.5f,0.5f,-1.0f });
	pointLight_ = std::make_unique<PointLight>();
	pointLight_->SetPosition({ 0.0f,0.0f,0.0f });
	pointLight_->SetRadius(6.0f);
	pointLight_->SetDecay(1.3f);
	pointLight_->SetIntensity(2.0f);

	// ライトを登録
	sceneLight_->SetLight(dirLight_.get());
	sceneLight_->SetLight(pointLight_.get());

	//ライト管理クラスの生成・初期化
	lightManager_ = std::make_unique<LightManager>();
	lightManager_->Initialize();
	lightManager_->SetPointLight(pointLight_.get());
	lightManager_->SetCamera(camera_.get());

	/* 天球の生成 + 初期化 */
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();
	//背景の生成 + 初期化
	background_ = std::make_unique<Object3d>();
	background_->Initialize(ShapeTag{}, "background", Shape::ShapeKind::kPlane);
	background_->SetTexture(TextureManager::GetInstance()->LoadTexture("backGround.png"));
	background_->SetIsLightProcess(true);
	backgroundWT_.Initialize();
	backgroundWT_.SetScale({ 150.0f, 100.0f, 1.0f });
	backgroundWT_.SetRotate({ 0.0f,-pi,0.0f });
	backgroundWT_.SetTranslate({ 0.0f, 0.0f, 2.5f });
	background_->RegistWorldTransform(&backgroundWT_);

	// Enemyの生成と初期化
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->SetPlayer(player_.get());
	enemyManager_->SetLightManager(lightManager_.get());

	// 爆発ギミック
	// ギミック管理
	gimmickManager_ = std::make_unique<GimmickManager>();
	gimmickManager_->Initialize(camera_.get());
	gimmickManager_->SetLightManager(lightManager_.get());
	
	/* ステージ管理クラス生成 + ステージ読み込み */
	stageManager_ = std::make_unique<StageManager>();
	stageManager_->LoadStage(
		"resources/stages/stage1.json", 
		player_.get(),
		enemyManager_.get(),
		gimmickManager_.get()
	);

	// ポーズメニュー生成 + 初期化
	pauseMenu_ = std::make_unique<PauseMenu>();
	pauseMenu_->Initialize();

	operationUI_ = std::make_unique<OperationUI>();
	operationUI_->Initialize(player_.get(), camera_.get(), Input::GetInstance());

	// BGM読み込み
	bgmGame_ = std::make_unique<Norm::Audio>();
	bgmGame_->Initialize("gameBGM.wav");
	bgmGame_->Play(true, 0.5f);

	//ポストエフェクト　ブルーム
	PostEffectManager::GetInstance()->AddPostEffectOrder(PostEffectKind::BloomExtract);
	PostEffectManager::GetInstance()->AddPostEffectOrder(PostEffectKind::GaussianFilter, PostEffectKind::BloomExtract);
	PostEffectManager::GetInstance()->AddPostEffectOrder(PostEffectKind::BloomComposite, PostEffectKind::GaussianFilter);

}

void Norm::GamePlayScene::Finalize() {}

void Norm::GamePlayScene::Update() {
	/* シーン共通更新処理 */
	BaseScene::Update();

	/* ポーズメニュー更新処理 */
	pauseMenu_->Update();

	operationUI_->Update();

	// タイトルへ戻るが押されていたらシーンを切り替える
	if (pauseMenu_->IsRequestedReturnToTitle()) {
		sceneManager_->SetNextScene("TITLE");
	}
	// ポーズ中なら以降の更新をスキップ
	if (pauseMenu_->IsPaused()) { 
		return;
	}

	//ライト管理クラスの更新
	lightManager_->Update();

	/* プレイヤー更新処理 */
	player_->Update();
	// ゴールに触れたらリザルトへ移行（仮）
	if (player_->IsGoaled()) {
		sceneManager_->SetNextScene("RESULT");
	} else if (player_->IsDead()) {
		sceneManager_->SetNextScene("GAMEOVER");
	}
	backgroundWT_.SetTranslate({ // 背景オブジェクトをプレイヤーに追従させる
		player_->GetTranslate().x, 
		player_->GetTranslate().y, 
		player_->GetTranslate().z + 4.0f // ちょっと奥に配置
	});
	/* カメラ更新処理 */
	camera_->Update();

	// Enemyの更新
	enemyManager_->UpdateEnemies();

	// 全ギミックを更新
	gimmickManager_->Update();

	/* 当たり判定処理（全ての移動が終わったあとのため最後）*/
	CollisionManager::GetInstance()->CheckCollision();
}

void Norm::GamePlayScene::DebugWithImGui() {
#ifdef _DEBUG
	/* カメラデバッグ用 */
	if (ImGui::Begin("Camera Debug")) {
		// 現在の値を取得
		Vector3 currentTranslate = camera_->worldTransform.GetTranslate();
		Vector3 currentRotate = camera_->worldTransform.GetRotate();

		// ImGuiで扱えるようfloatの配列で管理
		float translate[3] = { currentTranslate.x, currentTranslate.y, currentTranslate.z };
		float rotate[3] = { currentRotate.x, currentRotate.y, currentRotate.z };

		// DragFloat3で編集
		bool isChanged = false;
		if (ImGui::DragFloat3("Translate", translate, 0.1f)) {
			isChanged = true;
		}
		if (ImGui::DragFloat3("Rotation", rotate, 0.01f)) {
			isChanged = true;
		}

		// 値に変更があった場合のみ、Setterでカメラに書き戻す
		if (isChanged) {
			camera_->worldTransform.SetTranslate({ translate[0], translate[1], translate[2] });
			camera_->worldTransform.SetRotate({ rotate[0], rotate[1], rotate[2] });
		}
	}
	ImGui::End();

	/* カメラデバッグ */
	camera_->DebugWithImGui();

	//ライト管理クラスのデバッグ
	lightManager_->Debug();

	/* プレイヤーデバッグ */
	player_->Debug();

	/* ステージ管理クラスデバッグ */
	stageManager_->Debug();

	// Enemy用デバッグ
	enemyManager_->DebugWithImGui();

	//平行光源
	dirLight_->DebugWithImGui(L"平行光源１");
	//点光源
	pointLight_->DebugWithImGui(L"点光源１");

	if (gimmickManager_) {
		gimmickManager_->Debug();
	}
	background_->Debug(L"背景");

	//explosionGimmick_->DebugImGui();

	PostEffectManager::GetInstance()->DebugWithImGui();

#endif
}