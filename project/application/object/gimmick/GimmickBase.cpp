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

void GimmickBase::CreateCollider(CollisionAttribute attribute, const Vector3& offset, const Vector3& size)
{
	if (!gimmickObject_) 
	{
		return;
	}

	collider_ = std::make_unique<GimmickCollider>(this);

	auto* collider = dynamic_cast<GimmickCollider*>(collider_.get());
	if (!collider) 
	{
		return;
	}

	collider->SetCollisionAttribute(attribute);
	collider->SetWorldTransform(&worldTransform_);
	collider->SetOffset(offset);
	collider->SetOBBSize(size);

}

void GimmickBase::SetColliderSize(const Vector3& size)
{
	auto* collider = dynamic_cast<GimmickCollider*>(collider_.get());
	if (!collider) 
	{
		return;
	}

	collider->SetOBBSize(size);

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