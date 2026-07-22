#include "GimmickBase.h"

void GimmickBase::Update()
{
	HitLight();
}

void GimmickBase::UpdateUI() {

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