#include "MapChip.h"

// Engine
#include <CollisionManager.h>

// Appliation
#include <application/object/collision/ObjectCollider.h>

void Norm::MapChip::Initialize(Type type, const Vector3& position) { 
	type_ = type;
	object_ = std::make_unique<Object3d>();

	// タイプに応じてモデル名を切り替える予定
	std::string modelName = "normalBlock";

	/* オブジェクト生成 + 初期化 */
	object_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName(modelName), modelName);
	wt_.Initialize();
	wt_.SetTranslate(position);
	object_->RegistWorldTransform(&wt_);

	/* コライダーの適用処理 */
	if (type_ == Type::NormalBlock) {
		// コライダーを生成
		collider_ = std::make_unique<ObjectCollider>(object_.get());

		// ObjectColliderにダウンキャストして詳細設定
		auto* chipCollider = dynamic_cast<ObjectCollider*>(collider_.get());
		if (chipCollider) {
			// 衝突属性を設定
			chipCollider->SetCollisionAttribute(CollisionAttribute::Block);
			// 連動するトランスフォームを登録
			chipCollider->SetWorldTransform(&wt_);
			// コライダーの中心のズレ
			chipCollider->SetOffset({0.0f, 0.0f, 0.0f});
			// ブロックのサイズ
			chipCollider->SetOBBSize({1.0f, 1.0f, 1.0f});
			// 自身のポインタをセット
			chipCollider->SetHolder(this);
		}
	}
}

void Norm::MapChip::Debug() {
#ifdef _DEBUG
	// コライダーが存在する場合のみ処理
	if (collider_) {
		auto* chipCollider = dynamic_cast<ObjectCollider*>(collider_.get());
		if (chipCollider) {
			chipCollider->Debug();
		}
	}
#endif
}
