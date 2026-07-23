#include "ExplosionGimmick.h"
#include "Object3dManager.h"
#include "TextureManager.h"
#include "CombinedParticleManager.h"

#ifdef _DEBUG
#include "imgui.h"
#endif



void ExplosionGimmick::Initialize(Norm::BaseCamera* _camera) {

	gimmickState_ = GimmickState::Hidden;// 初期状態は見つかっていない状態

	isExploded_ = false;// 爆発していない状態
	explosionTimer_ = 0.0f;// 爆発演出用タイマー
	explosionDuration_ = 0.5f;// 爆発演出の時間
	explosionScale_ = 1.0f;// 爆発演出のスケール
	radius_ = 1.0f;

	baseColliderSize_ = { 1.0f, 1.0f, 1.0f };

	worldTransform_.Initialize();
	worldTransform_.SetTranslate(position_);

	explosionParticle_ = std::make_unique<CombinedParticle>();
	explosionParticle_->Initialize(CombinedParticleManager::GetInstance()->GenerateName("ExplosionGimmick"), "Explosion");

	// とりあえず見た目用オブジェクト
	gimmickObject_ = std::make_unique<Object3d>();
	gimmickObject_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("ExplosionGimmick"), "bomb");
	gimmickObject_->SetIsOutline(true);
	gimmickObject_->SetOutlineParam(TextureManager::GetInstance()->LoadTexture("green.png"), 1.1f);

	guideUI_ = std::make_unique<GuideUI>();
	guideUI_->Initialize(_camera, Input::GetInstance(), position_);

	gimmickObject_->RegistWorldTransform(&worldTransform_);

	// コライダーは一度作成しておくが、初期は判定をオフにしておく
	CreateCollider(
		CollisionAttribute::Gimmick,
		{ 0.0f, 1.0f, 0.0f },
		baseColliderSize_
	);
	if (collider_) {
		
		collider_->SetCollisionAttribute(Norm::CollisionAttribute::Nothingness);
	}
}

	// SE読み込み
	seBombClick_ = std::make_unique<Norm::Audio>();
	seBombClick_->Initialize("bombClick.wav");

	seExplosion_ = std::make_unique<Norm::Audio>();
	seExplosion_->Initialize("explosion.wav");
}

void ExplosionGimmick::Update() {
	constexpr float kDeltaTime = 1.0f / 60.0f;

	// 使用済み：再生成まで待つ
	if (gimmickState_ == GimmickState::Used) {

		respawnTimer_ += kDeltaTime;

		if (respawnTimer_ >= respawnDuration_) {
			Reset();
		}

		return;
	}

	// 再生成演出中
	if (gimmickState_ == GimmickState::Respawning) {

		respawnAnimationTimer_ += kDeltaTime;

		float t =
			respawnAnimationTimer_ / respawnAnimationDuration_;

		if (t > 1.0f) {
			t = 1.0f;
		}

		// 0 → 1まで徐々に大きくする
		respawnScale_ = t;

		worldTransform_.SetScale({
			respawnScale_,
			respawnScale_,
			respawnScale_
			});

		// 再生成完了
		if (respawnAnimationTimer_ >= respawnAnimationDuration_) {

			worldTransform_.SetScale({
				1.0f,
				1.0f,
				1.0f
				});

			// 重要：再出現時もコライダーは生成しない（爆破時のみ判定を有効にする方針）
			gimmickState_ = GimmickState::Hidden;
		}

		return;
	}


	// Base側でライト判定
	GimmickBase::Update();

	// 爆発中の演出
	if (gimmickState_ == GimmickState::Active) {
		if (explosionTimer_ == 0.0f) {
			TransformEuler transform = {
				{1,1,1},
				{0,0,0},
				worldTransform_.GetTranslate()
			};
			explosionParticle_->SetBaseTransform(transform);
			explosionParticle_->SetIsPlay(true);
		}

		explosionTimer_ += kDeltaTime;

		float t = explosionTimer_ / explosionDuration_;
		if (t > 1.0f) {
			t = 1.0f;
		}

		// 爆発っぽく大きくする
		explosionScale_ = 1.0f + t * (maxExplosionScale_ - 1.0f);
		worldTransform_.SetScale({ explosionScale_, explosionScale_, explosionScale_ });

		// コライダーのサイズも大きくする
		SetColliderSize({ baseColliderSize_.x * explosionScale_,baseColliderSize_.y * explosionScale_,baseColliderSize_.z * explosionScale_ });

		if (explosionTimer_ >= explosionDuration_) {

			gimmickState_ = GimmickState::Used;
			respawnTimer_ = 0.0f;

			// モデルを非表示
			if (gimmickObject_) {
				gimmickObject_->SetIsDisplay(false);
			}

			// コリジョン無効化
			if (collider_) {
				collider_->SetCollisionAttribute(Norm::CollisionAttribute::Gimmick);
			}
			//// コリジョンを消す
			//collider_.reset();

			// 爆発音再生
			seExplosion_->Play(false, 0.5f);
		}
	}

}

void ExplosionGimmick::UpdateUI() {

	guideUI_->Update(position_ + uiOffset_, respawnTimer_ / respawnDuration_);
}

void ExplosionGimmick::Reset()
{
	gimmickState_ = GimmickState::Respawning;

	isExploded_ = false;

	explosionTimer_ = 0.0f;
	respawnTimer_ = 0.0f;

	respawnAnimationTimer_ = 0.0f;
	respawnScale_ = 0.0f;

	// 最初は小さい状態
	worldTransform_.SetScale({
		respawnScale_,
		respawnScale_,
		respawnScale_
		});

	// モデル表示
	if (gimmickObject_) {
		gimmickObject_->SetIsDisplay(true);
	}


	

}

void ExplosionGimmick::DebugImGui() {
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

		if (collider_) {
			auto* Collider = dynamic_cast<GimmickCollider*>(collider_.get());
			if (Collider) {
				Collider->Debug();
			}
		}

		ImGui::DragFloat(
			"再出現までの時間",
			&respawnDuration_,
			0.1f,
			0.1f,
			30.0f
		);

		if (gimmickState_ == GimmickState::Used) {
			ImGui::Text(
				"Respawn Timer: %.2f / %.2f",
				respawnTimer_,
				respawnDuration_
			);
		}

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



#endif
}

void ExplosionGimmick::OnFlashHit() {
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

	// ここでコライダーを有効化（存在しなければ作成）
	if (!collider_) {
		CreateCollider(
			CollisionAttribute::Gimmick,
			{ 0.0f, 1.0f, 0.0f },
			baseColliderSize_
		);
	}
	if (collider_) {
		collider_->SetCollisionAttribute(Norm::CollisionAttribute::Gimmick);
	}
	// 爆弾クリック音再生
	seBombClick_->Play(false, 0.5f);
}