#include "ExplosionGimmick.h"
#include "Object3dManager.h"

#ifdef _DEBUG
#include "imgui.h"
#endif



void ExplosionGimmick::Initialize()
{
	gimmickState_ = GimmickState::Hidden;

	isExploded_ = false;
	explosionTimer_ = 0.0f;
	explosionDuration_ = 0.5f;
	explosionScale_ = 1.0f;

	radius_ = 1.0f;

	worldTransform_.Initialize();
	worldTransform_.SetTranslate(position_);

	// とりあえず見た目用オブジェクト
	gimmickObject_ = std::make_unique<Object3d>();
	gimmickObject_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("ExplosionGimmick"), "cube");

	gimmickObject_->RegistWorldTransform(&worldTransform_);

	
	CreateCollider(
		CollisionAttribute::Player,
		{ 0.0f, 1.0f, 0.0f },
		worldTransform_.GetScale()
	);
}

void ExplosionGimmick::Update()
{
	// Base側でライト判定
	GimmickBase::Update();

	// 爆発中の演出
	if (gimmickState_ == GimmickState::Active) {
		explosionTimer_ += 1.0f / 60.0f;

		float t = explosionTimer_ / explosionDuration_;
		if (t > 1.0f) {
			t = 1.0f;
		}

		// 爆発っぽく大きくする
		explosionScale_ = 1.0f + t * (maxExplosionScale_ - 1.0f);
		worldTransform_.SetScale({ explosionScale_, explosionScale_, explosionScale_ });

		if (explosionTimer_ >= explosionDuration_) {
			gimmickState_ = GimmickState::Used;
			if (gimmickObject_) {
				gimmickObject_->SetIsDisplay(false);
			}
		}
	}
}
void ExplosionGimmick::Reset()
{
	gimmickState_ = GimmickState::Hidden;
	isExploded_ = false;
	explosionTimer_ = 0.0f;
	explosionScale_ = 1.0f;
	worldTransform_.SetScale({ explosionScale_, explosionScale_, explosionScale_ });
	gimmickObject_->SetIsDisplay(true);

}

void ExplosionGimmick::DebugImGui()
{
#ifdef _DEBUG
	if (ImGui::TreeNode("ExplosionGimmick")) {

		ImGui::Text("State: %s", GetStateName());
		ImGui::Text("IsExploded: %s", isExploded_ ? "true" : "false");

		ImGui::Separator();

		ImGui::Text("Position");
		ImGui::DragFloat3("Position", &position_.x, 0.1f);

		ImGui::DragFloat("爆発の時間", &explosionDuration_, 0.01f, 0.01f, 10.0f);
		ImGui::DragFloat("爆発の大きさ", &maxExplosionScale_, 0.1f, 1.0f, 30.0f);
		ImGui::Text("Explosion Timer: %.2f", explosionTimer_);

		if (ImGui::Button("Apply Position")) {
			worldTransform_.SetTranslate(position_);
		}

		if (ImGui::Button("Explosion Test")) {
			OnFlashHit();
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset")) {
			Reset();
		}

		ImGui::TreePop();

	}
	gimmickObject_->Debug(L"GimmickObjct");
	auto* collider1 = dynamic_cast<ObjectCollider*>(collider_.get());
	collider1->Debug();
	
#endif
}

void ExplosionGimmick::OnFlashHit()
{
	if (gimmickState_ == GimmickState::Used) {
		return;
	}

	if (isExploded_) {
		return;
	}

	isExploded_ = true;
	gimmickState_ = GimmickState::Active;

	explosionTimer_ = 0.0f;
	explosionScale_ = 1.0f;

	
}