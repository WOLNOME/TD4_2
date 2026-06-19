#include "IBaseLevelObject.h"
#include <CollisionManager.h>

using namespace Norm;

void IBaseLevelObject::Initialize(const std::string& _name) {
	//名前のセット
	name_ = _name;
	//オブジェクトの生成
	object3d_ = std::make_unique<Object3d>();
	object3d_->SetIsDisplay(false);
}

void IBaseLevelObject::Update() {

}

void IBaseLevelObject::DebugWithImGui() {
#ifdef _DEBUG

	// コライダーのデバッグ処理
	for (auto& [handle, collider] : colliders_) {
		if (collider) {
			collider->Debug();
		}
	}

#endif // _DEBUG
}

uint32_t IBaseLevelObject::SetTransformInfo(const Norm::TransformEuler& _transform) {
	uint32_t handle = 0u;

	//ワールドトランスフォームを定義
	std::unique_ptr<WorldTransform> worldTransform;
	worldTransform = std::make_unique<WorldTransform>();
	worldTransform->Initialize();
	worldTransform->SetTranslate(_transform.translate);
	worldTransform->SetRotate(_transform.rotate);
	worldTransform->SetScale(_transform.scale);

	//オブジェクトに登録
	handle = object3d_->RegistWorldTransform(worldTransform.get());

	//ワールドトランスフォームに保存
	worldTransforms_[handle] = std::move(worldTransform);

	//表示
	object3d_->SetIsDisplay(true);

	return handle;
}

void IBaseLevelObject::SetCollisionInfo(uint32_t _handle, const Vector3& _center, const Vector3& _size) {
	//当たり判定を定義
	std::unique_ptr<LevelObjectCollider> collider = nullptr;
	collider = std::make_unique<LevelObjectCollider>(this);
	collider->SetOffset(_center / 2.0f);
	collider->SetOBBSize(_size / 2.0f);
	collider->SetCollisionAttribute(CollisionAttribute::Nothingness);	//レベルオブジェクト専用の当たり判定を用意する。

	//ワールドトランスフォームをセット
	collider->SetWorldTransform(worldTransforms_[_handle].get());

	//コライダーに保存
	colliders_[_handle] = std::move(collider);
	colliders_[_handle]->SetHandle(_handle);

}