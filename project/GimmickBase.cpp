#include "GimmickBase.h"
#include <cassert>

//アプリケーション
#include <application/system/LightManager.h>

//エンジン
#include <PointLight.h>

void GimmickBase::Update()
{
	assert(lightManager_ && "ライトマネージャーが設定されていません");

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
	if (lightManager_->GetIsFlush()) {
		OnFlashHit();
		return;
	}

	//ライトが当たている
	OnLightHit();
}

bool GimmickBase::IsHitLightCircle() const
{
	Vector3 diff = position_ - lightManager_->GetPointLight()->GetPosition();

	float distanceSq =
		diff.x * diff.x +
		diff.y * diff.y +
		diff.z * diff.z;

	float hitRange = lightManager_->GetPointLight()->GetRadius() + radius_;

	return distanceSq <= hitRange * hitRange;
}