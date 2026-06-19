#include "ICollider.h"
#include "CollisionManager.h"
#include <SceneManager.h>

using namespace Norm;

ICollider::ICollider() {
	//シーンタグを設定
	sceneTag_ = SceneManager::GetInstance()->GetCurrentScene()->GetSceneName();

	//当たり判定マネージャーに登録
	CollisionManager::GetInstance()->RegistCollider(this);
}

ICollider::~ICollider() {
	//当たり判定マネージャーの登録を解除
	CollisionManager::GetInstance()->DeleteCollider(this);
}

Vector3 ICollider::GetCenter() {
	//オフセットをワールド空間に（平行移動・スケールは考慮しない）
	Matrix4x4 rotateMat = MyMath::MakeRotateMatrix(worldTransform_->GetRotate());
	Vector3 worldOffset = MyMath::TransformNormal(offset_, rotateMat);
	
	return worldTransform_->GetTranslate() + worldOffset;
}