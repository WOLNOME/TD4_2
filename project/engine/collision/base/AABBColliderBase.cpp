#include "AABBColliderBase.h"
#ifdef _DEBUG
#include <ImGuiManager.h>
#endif // _DEBUG

Norm::AABBColliderBase::AABBColliderBase() : ICollider() {
	//コライダーの形状を設定
	colliderShape_ = ColliderShape::AABB;
}

void Norm::AABBColliderBase::Debug() {
#ifdef _DEBUG
	//Nothingなら描画しない
	if (GetCollisionAttribute() == CollisionAttribute::Nothingness) {
		return;
	}
	//AABBを定義
	AABB aabb = {
		.min = aabb_.min + GetWorldTransform().GetTranslate(),
		.max = aabb_.max + GetWorldTransform().GetTranslate()
	};
	MyMath::CreateLineAABB(aabb, debugLineColor_);

#endif // _DEBUG
}
