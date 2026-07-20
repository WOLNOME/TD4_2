#include "FollowCamera.h"

// externals
#include <imgui.h>

// Appliaction
#include <application/object/Character/Player.h>

void Norm::FollowCamera::Initialize(const Player* target) { 
	// 基底クラスの初期化
	BaseCamera::Initialize();

	target_ = target; 

	if (target_) {
		Vector3 initialPos = target_->GetTranslate() + offset_;
		worldTransform.SetTranslate(initialPos);
		worldTransform.UpdateMatrix();
	}
}

void Norm::FollowCamera::Update() {
	if (!target_) return;

	// 目標となるカメラの位置を計算
	Vector3 targetPos = target_->GetTranslate() + offset_;

	// 現在のカメラ位置から、目標位置へ向かって線形補間
	Vector3 currentPos = worldTransform.GetTranslate();
	Vector3 newPos = MyMath::Lerp(currentPos, targetPos, interpolationRate_);

	// 計算した新しい座標をカメラにセット
	worldTransform.SetTranslate(newPos);

	// 基底クラスの更新
	BaseCamera::Update();
}

void Norm::FollowCamera::DebugWithImGui() {
	if (ImGui::Begin("Follow Camera Debug")) {
		ImGui::DragFloat3("Offset", &offset_.x, 1.0f);
		ImGui::SliderFloat("Inertia", &interpolationRate_, 0.01f, 1.0f, "%.2f");
	}
	ImGui::End();
}