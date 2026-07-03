#include "Player.h"

// Engine
#include <Object3dManager.h>
#include <imgui.h>

void Norm::Player::Initialize() {
	// インプットのインスタンス取得
	input_ = Input::GetInstance();

	// 3Dモデルの生成 + 初期化
	object_ = std::make_unique<Object3d>();
	object_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("player"), "player");
	wt_.Initialize();
	wt_.SetTranslate({0.0f, 0.0f, 0.0f});
	object_->RegistWorldTransform(&wt_);
}

void Norm::Player::Update() {
	// 入力の受付
	velocity_ = {0.0f, 0.0f, 0.0f};
	if (input_->PushKey(DIK_A)) {
		velocity_.x = -speed_;
	}
	if (input_->PushKey(DIK_D)) {
		velocity_.x = speed_;
	}

	// 座標の更新
	Vector3 currentPos = wt_.GetTranslate();
	wt_.SetTranslate({ 
		currentPos.x + velocity_.x, 
		currentPos.y + velocity_.y, 
		currentPos.z + velocity_.z
	});
}

void Norm::Player::Debug() { 
#ifdef _DEBUG
	if (ImGui::Begin("Player")) {
		// 現在の値を取得
		Vector3 playerTranslate = wt_.GetTranslate();

		// ImGuiで扱えるようfloatの配列で管理
		float translate[3] = {playerTranslate.x, playerTranslate.y, playerTranslate.z};

		// DragFloat3で編集
		bool isChanged = false;
		if (ImGui::DragFloat3("Translate", translate, 0.1f)) {
			isChanged = true;
		}

		// 値に変更があった場合のみ、Setterでカメラに書き戻す
		if (isChanged) {
			wt_.SetTranslate({translate[0], translate[1], translate[2]});
		}
	}
	ImGui::End();
#endif
}