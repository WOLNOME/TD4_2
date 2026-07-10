#include "GimmickBase.h"

void GimmickBase::Update()
{
	HitLight();

	guideUI_->SetPosition(position_ + uiOffset_);
	guideUI_->Update();
}

void GimmickBase::HitLight()
{
	if (!IsHitLightCircle()) {
		return;
	}

	// 最初にライトで見つかった
	if (gimmickState_ == GimmickState::Hidden) {
		gimmickState_ = GimmickState::Discovered;
	}

	// フラッシュだったらフラッシュ用処理
	if (lightInfo_->isFlash) {
		OnFlashHit();
		return;
	}

	//ライトが当たている
	OnLightHit();
}

bool GimmickBase::IsHitLightCircle() const
{
	if (!lightInfo_) {
		return false;
	}

	if (!lightInfo_->isLighting) {
		return false;
	}

	Vector3 diff = position_ - lightInfo_->position;

	float distanceSq =
		diff.x * diff.x +
		diff.y * diff.y +
		diff.z * diff.z;

	float hitRange = lightInfo_->range + radius_;

	return distanceSq <= hitRange * hitRange;
}