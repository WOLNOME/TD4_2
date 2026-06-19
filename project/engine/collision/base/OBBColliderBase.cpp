#include "OBBColliderBase.h"
#include <MyMath.h>
#ifdef _DEBUG
#include <ImGuiManager.h>
#endif // _DEBUG

using namespace Norm;

OBBColliderBase::OBBColliderBase() : ICollider() {
	//コライダーの形状を設定
	colliderShape_ = ColliderShape::OBB;
}

void OBBColliderBase::Debug() {
#ifdef _DEBUG
	//Nothingなら描画しない
	if (GetCollisionAttribute() == CollisionAttribute::Nothingness) {
		return;
	}

	//回転行列
	Matrix4x4 matRotate = MyMath::MakeRotateMatrix(GetWorldTransform().GetRotate());
	//OBBを定義
	OBB obb = {
		.center = GetWorldTransform().GetWorldTranslate() + offset_,
		.orientations = {MyMath::TransformNormal(Vector3(1,0,0),matRotate),MyMath::TransformNormal(Vector3(0,1,0),matRotate),MyMath::TransformNormal(Vector3(0,0,1),matRotate)},
		.size = size_
	};
	//線を登録
	MyMath::CreateLineOBB(obb, debugLineColor_);

#endif // _DEBUG
}

OBB OBBColliderBase::GetOBB() {
	//回転行列
	Matrix4x4 matRotate = MyMath::MakeRotateMatrix(worldTransform_->GetRotate());

	//OBBを作成
	OBB result;
	result.center = GetCenter();
	result.orientations[0] = MyMath::TransformNormal(Vector3(1, 0, 0), matRotate);
	result.orientations[1] = MyMath::TransformNormal(Vector3(0, 1, 0), matRotate);
	result.orientations[2] = MyMath::TransformNormal(Vector3(0, 0, 1), matRotate);
	result.size = size_;

	return result;
}
