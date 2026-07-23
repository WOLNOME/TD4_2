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
	pointLight_ = std::make_unique<PointLight>();
	pointLight_->SetPosition({ 0.0f,0.0f,0.0f });
	pointLight_->SetRadius(15.0f);
	pointLight_->SetDecay(1.0f);

	//ライトをシーンに登録
	sceneLight_->SetLight(pointLight_.get());

	/* 背景オブジェクトの生成 */
	backGroundObject_ = std::make_unique<Object3d>();
	backGroundObject_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("backGround"), "backGroundCube");

	//ワールドトランスフォームを初期化
	backGroundWT_.Initialize();
	backGroundWT_.SetTranslate({ 0.0f,-50.0f,10.0f });
	backGroundWT_.SetScale({ 100.0f,100.0f,1.0f });

	//背景オブジェクトに登録
	backGroundObject_->RegistWorldTransform(&backGroundWT_);

	/* UIの初期化 */
	titleUI_.textureHandle = TextureManager::GetInstance()->LoadTexture("title.png");
	titleUI_.sprite = std::make_unique<Sprite>();
	titleUI_.sprite->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("titleUI"), Order::Front2, titleUI_.textureHandle);
	titleUI_.sprite->SetAnchorPoint({ 0.5f,0.5f });
	titleUI_.sprite->SetPosition(titlePos_);

	buttonUI_.textureHandle = TextureManager::GetInstance()->LoadTexture("titleButton.png");
	buttonUI_.sprite = std::make_unique<Sprite>();
	buttonUI_.sprite->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("buttonUI"), Order::Front2, buttonUI_.textureHandle);
	buttonUI_.sprite->SetAnchorPoint({ 0.5f,0.5f });
	buttonUI_.sprite->SetPosition(buttonPos_);

	tutorialButtonUI_.textureHandle = TextureManager::GetInstance()->LoadTexture("tutorialButton.png");
	tutorialButtonUI_.sprite = std::make_unique<Sprite>();
	tutorialButtonUI_.sprite->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("tutorialButtonUI"), Order::Front2, tutorialButtonUI_.textureHandle);
	tutorialButtonUI_.sprite->SetAnchorPoint({ 0.5f,0.5f });
	tutorialButtonUI_.sprite->SetPosition(tutorialButtonPos_);

	tutorialUI_.textureHandle = TextureManager::GetInstance()->LoadTexture("tutorial.png");
	tutorialUI_.sprite = std::make_unique<Sprite>();
	tutorialUI_.sprite->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("tutorialUI"), Order::Front3, tutorialUI_.textureHandle);
	tutorialUI_.sprite->SetAnchorPoint({ 0.5f,0.5f });
	tutorialUI_.sprite->SetPosition(tutorialPos_);

	exitUI_.textureHandle = TextureManager::GetInstance()->LoadTexture("key_escape.png");
	exitUI_.sprite = std::make_unique<Sprite>();
	exitUI_.sprite->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("exitUI"), Order::Front3, exitUI_.textureHandle);
	exitUI_.sprite->SetAnchorPoint({ 0.5f,0.5f });
	exitUI_.sprite->SetPosition(exitUIPos_);

	//ボタンUIの初期サイズを取得
	buttonInitSize_ = buttonUI_.sprite->GetSize();

	/* 演出用エネミーの生成 */
	for (int i = 0; i < maxEnemy_; i++) {

		std::unique_ptr<TitleEnemy> newObject = std::make_unique<TitleEnemy>();

		newObject->Initialize(Vector3(spawnLengthX_ * spawnDirection_, spawnLengthY_ * i + startPosY_, 2.0f));

		spawnDirection_ *= -1.0f;

		//エネミーが保持しているライトをシーンに登録
		sceneLight_->SetLight(newObject->GetPointLight());

		titleEnemies_.push_back(std::move(newObject));

	}

	buttonInitSize_ = buttonUI_.sprite->GetSize();

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

	//カメラの更新
	camera_->Update();

	//ライトの追従処理
	LightMoveProcess();

	//背景オブジェクトの座標更新
	backGroundWT_.UpdateMatrix();

	//演出用エネミーの更新
	for (auto& titleEnemy : titleEnemies_) {

		//シーンチェンジが始まったら逃げ出す処理を開始する
		titleEnemy->SetIsRun(isSceneChange_);

		titleEnemy->Update();
	}

	/* ボタンUIの更新 */
	Vector3 mousePos = { Input::GetInstance()->GetMousePosition().x,Input::GetInstance()->GetMousePosition().y,0.0f };

	Vector3 buttonPos = { buttonUI_.sprite->GetPosition().x,buttonUI_.sprite->GetPosition().y,0.0f };

	Vector3 tutorialButtonPos = { tutorialButtonUI_.sprite->GetPosition().x,tutorialButtonUI_.sprite->GetPosition().y,0.0f };

	if (isTutorial_) {

		tutorialTimer_ += 1.0f / 60.0f;

		if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {

			seClick_->Play(false, 0.5f);

			isTutorial_ = false;
		}
	} else {

		tutorialTimer_ -= 1.0f / 60.0f;

		//マウスがボタンUIの範囲内にあれば
		if (MyMath::Length(mousePos - buttonPos) <= buttonLength_) {

			//サイズを少し大きくする
			buttonUI_.sprite->SetSize(buttonInitSize_ * buttonSizeRatio_);

			//範囲内で左クリックが押されたらシーンチェンジを開始する
			if (Input::GetInstance()->TriggerMouseButton(MouseButton::LeftButton)) {

				// クリック音再生
				seClick_->Play(false, 0.5f);

				isSceneChange_ = true;
			}

		} else {

			buttonUI_.sprite->SetSize(buttonInitSize_);
		}

		if (MyMath::Length(mousePos - tutorialButtonPos) <= buttonLength_) {

			tutorialButtonUI_.sprite->SetSize(buttonInitSize_ * buttonSizeRatio_);

			if (Input::GetInstance()->TriggerMouseButton(MouseButton::LeftButton)) {

				seClick_->Play(false, 0.5f);

				isTutorial_ = true;
			}
		} else {

			tutorialButtonUI_.sprite->SetSize(buttonInitSize_);
		}
	}

	tutorialTimer_ = std::clamp(tutorialTimer_, 0.0f, tutorialMaxTime_);

	tutorialUI_.sprite->SetColor({ 1.0f,1.0f,1.0f,MyMath::Lerp(0.0f, 1.0f, tutorialTimer_ / tutorialMaxTime_) });

	exitUI_.sprite->SetColor({ 1.0f,1.0f,1.0f,MyMath::Lerp(0.0f, 1.0f, tutorialTimer_ / tutorialMaxTime_) });

	if (isSceneChange_) {

		//シーンチェンジ開始
		sceneChangeTimer_ += 1.0f / 60.0f;

		//ライトをフラッシュさせて少しずつ減衰させる
		float easeT = MyMath::EaseOutQuart(sceneChangeTimer_ / sceneChangeMaxTime_);

		pointLight_->SetRadius(MyMath::Lerp(lightRadiusBefore_, lightRadiusAfter_, easeT));

		pointLight_->SetIntensity(MyMath::Lerp(lightIntensityBefore_, lightIntensityAfter_, easeT));
	}

	if (sceneChangeTimer_ >= sceneChangeMaxTime_) {

		//演出が終わったらシーンチェンジ
		sceneChangeTimer_ = sceneChangeMaxTime_;

		sceneManager_->SetNextScene("GAMEPLAY");
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