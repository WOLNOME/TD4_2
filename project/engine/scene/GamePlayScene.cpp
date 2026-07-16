#include "GamePlayScene.h"

// Engine
#include <CollisionManager.h>
#include <Input.h>

void Norm::GamePlayScene::Initialize() {
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
	dirLight_->SetColor({ 1,0,0,1 });
	dirLight_->SetDirection({ 0.5f,0.5f,-1.0f });
	pointLight_ = std::make_unique<PointLight>();
	pointLight_->SetPosition({ 0.0f,0.0f,0.0f });
	// ライトを登録
	sceneLight_->SetLight(dirLight_.get());
	sceneLight_->SetLight(pointLight_.get());

	/* 天球の生成 + 初期化 */
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	/* ステージ管理クラス生成 + ステージ読み込み */
	stageManager_ = std::make_unique<StageManager>();
	stageManager_->LoadStage("resources/stages/stage1.json");

	/* プレイヤー生成 + 初期化 */
	player_ = std::make_unique<Player>();
	player_->Initialize();

	// Enemyの生成と初期化
	enemy_ = std::make_unique<BaseEnemy>();
	enemy_->Initialize({ 25.0f, -25.0f, 0.0f });

	// 爆発ギミック
	explosionGimmick_ = std::make_unique<ExplosionGimmick>();
	explosionGimmick_->SetLightInfo(&lightInfo_);
	explosionGimmick_->SetPosition({ 14.0f,-25.0f, 0.0f });
	explosionGimmick_->Initialize(camera_.get());
}

void Norm::GamePlayScene::Finalize() {}

void Norm::GamePlayScene::Update() {
	/* シーン共通更新処理 */
	BaseScene::Update();
	/* カメラ更新処理 */
	camera_->Update();
	//ライト移動処理
	LightMoveProcess();

	/* プレイヤー更新処理 */
	player_->Update();

	// Enemyの更新
	enemy_->Update();

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

	/* プレイヤーデバッグ用 */
	player_->Debug();

	/* ステージ管理クラスデバッグ用 */
	stageManager_->Debug();

	// Enemy用デバッグ
	enemy_->DebugWithImGui();

	//平行光源
	dirLight_->DebugWithImGui(L"平行光源１");
	//点光源
	pointLight_->DebugWithImGui(L"点光源１");

	explosionGimmick_->DebugImGui();


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
	XYPlane.distance = 0.0f;
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