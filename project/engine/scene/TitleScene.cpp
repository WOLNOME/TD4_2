#include "TitleScene.h"

#include "engine/scenebase/SceneManager.h"
#include "engine/base/texture/TextureManager.h"
#include "engine/2d/SpriteManager.h"
#include "engine/3d/object/Object3dManager.h"
#include "Input.h"

void Norm::TitleScene::Initialize() {

	BaseScene::Initialize();

	/* 開発用カメラ生成 + 初期化 */
	camera_ = std::make_unique<DevelopCamera>();
	camera_->Initialize();
	camera_->SetFarClip(1000.0f);
	camera_->worldTransform.SetTranslate(cameraTranslate_);
	camera_->worldTransform.SetRotate(cameraRotate_);
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
	pointLight_->SetRadius(10.0f);

	sceneLight_->SetLight(dirLight_.get());
	sceneLight_->SetLight(pointLight_.get());

	object_ = std::make_unique<Object3d>();
	object_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("backGround"), "cube");
	object_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	wt_.Initialize();
	wt_.SetTranslate({ 0.0f,-50.0f,10.0f });
	wt_.SetScale({ 100.0f,100.0f,1.0f });
	object_->RegistWorldTransform(&wt_);

	titleUI_.textureHandle = TextureManager::GetInstance()->LoadTexture("whiteSquare.png");
	titleUI_.sprite = std::make_unique<Sprite>();
	titleUI_.sprite->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("titleUI"), Order::Front2, titleUI_.textureHandle);
	titleUI_.sprite->SetAnchorPoint({ 0.5f,0.5f });
	titleUI_.sprite->SetPosition({ 640.0f,180.0f });

	buttonUI_.textureHandle = TextureManager::GetInstance()->LoadTexture("whiteSquare.png");
	buttonUI_.sprite = std::make_unique<Sprite>();
	buttonUI_.sprite->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("buttonUI"), Order::Front2, buttonUI_.textureHandle);
	buttonUI_.sprite->SetAnchorPoint({ 0.5f,0.5f });
	buttonUI_.sprite->SetPosition({ 640.0f,360.0f });
	buttonUI_.sprite->SetSize({ 100.0f,75.0f });

	initSize_ = buttonUI_.sprite->GetSize();

	// 音声読み込み
	seClick_ = std::make_unique<Norm::Audio>();
	seClick_->Initialize("click.wav");

	bgmTitle_ = std::make_unique<Norm::Audio>();
	bgmTitle_->Initialize("titleBGM.wav");
	bgmTitle_->Play(true, 0.5f);

	PostEffectManager::GetInstance()->AddPostEffectOrder(PostEffectKind::None);
}

void Norm::TitleScene::Finalize() {
}

void Norm::TitleScene::Update() {
	/* シーン共通更新処理 */
	BaseScene::Update();

	camera_->Update();

	LightMoveProcess();

	wt_.UpdateMatrix();

	Vector2 mousePos = Input::GetInstance()->GetMousePosition();

	Vector2 uiPos = buttonUI_.sprite->GetPosition();

	if (MyMath::Length(Vector3(mousePos.x, mousePos.y, 0.0f) - Vector3(uiPos.x, uiPos.y, 0.0f)) <= 10.0f) {

		buttonUI_.sprite->SetSize(initSize_ * 1.2f);

		if (Input::GetInstance()->TriggerMouseButton(MouseButton::LeftButton)) {
			// クリック音再生
			seClick_->Play(false, 0.5f);

			sceneManager_->SetNextScene("GAMEPLAY");
		}
	} else {

		buttonUI_.sprite->SetSize(initSize_);
	}

}

void Norm::TitleScene::DebugWithImGui() {
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
	//平行光源
	dirLight_->DebugWithImGui(L"平行光源１");
	//点光源
	pointLight_->DebugWithImGui(L"点光源１");

#endif
}

void Norm::TitleScene::LightMoveProcess() {
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
}