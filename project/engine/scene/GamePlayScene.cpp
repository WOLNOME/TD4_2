#include "GamePlayScene.h"

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
	dirLight_->SetIntensity(1.0f);
	dirLight_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	dirLight_->SetDirection({0.0f, -1.0f, 0.0f});
	// ライトを登録
	sceneLight_->SetLight(dirLight_.get());

	/* 天球の生成 + 初期化 */
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	/* テスト用オブジェクト生成 */
	object_ = std::make_unique<Object3d>();
	object_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("normalBlock"), "normalBlock");
	objectWT_.Initialize();
	objectWT_.SetTranslate({0.0f, 0.0f, 0.0f});
	objectWT_.SetRotate({0.0f, 0.0f, 0.0f});
	object_->RegistWorldTransform(&objectWT_);
}

void Norm::GamePlayScene::Finalize() {}

void Norm::GamePlayScene::Update() {
	/* シーン共通更新処理 */
	BaseScene::Update();
	/* カメラ更新処理 */
	camera_->Update();
}

void Norm::GamePlayScene::DebugWithImGui() {
#ifdef _DEBUG
	/* カメラデバッグ用 */
	if (ImGui::Begin("Camera Debug")) {
		// 現在の値を取得
		Vector3 currentTranslate = camera_->worldTransform.GetTranslate();
		Vector3 currentRotate = camera_->worldTransform.GetRotate();

		// ImGuiで扱えるようfloatの配列で管理
		float translate[3] = {currentTranslate.x, currentTranslate.y, currentTranslate.z};
		float rotate[3] = {currentRotate.x, currentRotate.y, currentRotate.z};

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
			camera_->worldTransform.SetTranslate({translate[0], translate[1], translate[2]});
			camera_->worldTransform.SetRotate({rotate[0], rotate[1], rotate[2]});
		}
	}
	ImGui::End();
#endif
}
