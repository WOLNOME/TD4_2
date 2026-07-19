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
	pointLight_->SetRadius(8.0f);
	pointLight_->SetDecay(1.5f);
	pointLight_->SetIntensity(3.0f);

	// ライトを登録
	sceneLight_->SetLight(dirLight_.get());
	sceneLight_->SetLight(pointLight_.get());

	/* 天球の生成 + 初期化 */
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();
	//背景の生成 + 初期化
	background_ = std::make_unique<Object3d>();
	background_->Initialize(ShapeTag{},"background",Shape::ShapeKind::kPlane);
	background_->SetTexture(TextureManager::GetInstance()->LoadTexture("backGround.png"));
	background_->SetIsLightProcess(true);
	backgroundWT_.Initialize();
	backgroundWT_.SetScale({ 100.0f, 100.0f, 1.0f });
	backgroundWT_.SetRotate({ 0.0f,-pi,0.0f });
	backgroundWT_.SetTranslate({ 0.0f, 0.0f, 2.5f });
	background_->RegistWorldTransform(&backgroundWT_);

	/* ステージ管理クラス生成 + ステージ読み込み */
	stageManager_ = std::make_unique<StageManager>();
	stageManager_->LoadStage("resources/stages/stage1.json");

	// Enemyの生成と初期化
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->SpawnEnemy({ 25.0f, -25.0f, 0.0f }, player_.get());

	// 爆発ギミック
	explosionGimmick_ = std::make_unique<ExplosionGimmick>();
	explosionGimmick_->SetLightInfo(&lightInfo_);
	explosionGimmick_->SetPosition({ 14.0f,-25.0f, 0.0f });
	explosionGimmick_->Initialize(camera_.get());

	// 背景オブジェクト生成 + 初期化
	background_ = std::make_unique<Object3d>();
	background_->Initialize(ShapeTag{}, Object3dManager::GetInstance()->GenerateName("background"), Shape::ShapeKind::kPlane);
	background_->SetColor({0.2f, 0.2f, 0.2f, 1.0f});
	backgroundWT_.Initialize();
	backgroundWT_.SetScale({150.0f, 100.0f, 1.0f}); // 画面全体を覆うように
	background_->RegistWorldTransform(&backgroundWT_);

	// ポーズメニュー生成 + 初期化
	pauseMenu_ = std::make_unique<PauseMenu>();
	pauseMenu_->Initialize();

	//ポストエフェクト　ブルーム
	//PostEffectManager::GetInstance()->AddPostEffectOrder(PostEffectKind::None);
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
	// タイトルへ戻るが押されていたらシーンを切り替える
	if (pauseMenu_->IsRequestedReturnToTitle()) {
		sceneManager_->SetNextScene("TITLE");
	}
	// ポーズ中なら以降の更新をスキップ
	if (pauseMenu_->IsPaused()) { 
		return;
	}

	//ライト移動処理
	LightMoveProcess();

	/* プレイヤー更新処理 */
	player_->Update();
	// ゴールに触れたらリザルトへ移行（仮）
	if (player_->IsGoaled()) {
		sceneManager_->SetNextScene("RESULT");
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

	//爆発ギミック
	explosionGimmick_->Update();

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

	background_->Debug(L"背景");

	explosionGimmick_->DebugImGui();

	PostEffectManager::GetInstance()->DebugWithImGui();

#endif
}

void Norm::GamePlayScene::LightMoveProcess() {
	//インプットの取得
	auto* input = Input::GetInstance();

	//ベクトル1を求める
	Vector3 cameraPos = camera_->worldTransform.GetWorldTranslate();
	Vector3 pointX;	//マウスのスクリーン座標をワールド座標に変換したときのある点
	Vector3 mousePos = { input->GetMousePosition().x,input->GetMousePosition().y,0.0f };
	float ndcX = (2.0f * mousePos.x / WinApp::GetInstance()->kClientWidth) - 1.0f;
	float ndcY = 1.0f - (2.0f * mousePos.y / WinApp::GetInstance()->kClientHeight);
	Vector3 pointNDC =
	{
		ndcX,
		ndcY,
		1.0f
	};
	Matrix4x4 invViewProj =
		MyMath::Inverse(camera_->GetViewProjectionMatrix());
	pointX = MyMath::Transform(pointNDC, invViewProj);
	//ベクトル1を直線に変換
	Line line;
	line.diff = Vector3(pointX - cameraPos).Normalized();
	line.origin = cameraPos;
	//XY平面を作成
	Plane XYPlane;
	XYPlane.normal = { 0,0,1 };
	XYPlane.distance = -2.0f;
	//直線と平面の交点CPを求める
	Vector3 cp = MyMath::CollisionPoint(line, XYPlane);
	//点光源の座標としてcpを適用する
	pointLight_->SetPosition(cp);

	// ギミック判定用ライト情報
	lightInfo_.position = cp;
	lightInfo_.range = pointLight_->GetRadius();
	lightInfo_.isLighting = true;

	// 左クリックでフラッシュ
	lightInfo_.isFlash = input->TriggerMouseButton(MouseButton::LeftButton);

}