#include "SphereColliderBase.h"
#ifdef _DEBUG
#include <ImGuiManager.h>
#endif // _DEBUG

using namespace Norm;

SphereColliderBase::SphereColliderBase() : ICollider() {
	//コライダーの形状を設定
	colliderShape_ = ColliderShape::Sphere;
}

void SphereColliderBase::Debug() {
#ifdef _DEBUG
	//Nothingなら描画しない
	if (GetCollisionAttribute() == CollisionAttribute::Nothingness) {
		return;
	}
	//球体を定義
	Sphere sphere = {
		.center = GetWorldTransform().GetWorldTranslate(),
		.radius = radius_
	};
	MyMath::CreateLineSphere(sphere, debugLineColor_);

#endif // _DEBUG
}

Capsule SphereColliderBase::GetCapsule() {
	//カプセルを作成
	Capsule result;
	result.radius = radius_;
	result.segment.origin = worldTransform_->GetPreWorldTranslate();
	result.segment.diff =
		worldTransform_->GetWorldTranslate()
		- worldTransform_->GetPreWorldTranslate();

	return result;
}
