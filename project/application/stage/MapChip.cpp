#include "MapChip.h"

// Engine
#include <CollisionManager.h>

// Appliation
#include <application/object/collision/ObjectCollider.h>

void Norm::MapChip::Initialize(Type type, const Vector3& position) {
	type_ = type;
	object_ = std::make_unique<Object3d>();

	// タイプに応じてモデル名を切り替える
	std::string modelName = "normalBlock";
	CollisionAttribute attribute = CollisionAttribute::Block; // デフォルトは通常ブロックにしておく

	if (type_ == MapChip::Type::NormalBlock) {
		modelName = "normalBlock";
		attribute = CollisionAttribute::Block;
	} else if (type_ == MapChip::Type::AreaBlock) {
		modelName = "areaBlock";
		attribute = CollisionAttribute::Area;
	} else if (type_ == MapChip::Type::GoalBlock) {
		modelName = "goalBlock";
		attribute = CollisionAttribute::Goal;
	}

	/* オブジェクト生成 + 初期化 */
	object_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName(modelName), modelName);
	wt_.Initialize();
	wt_.SetTranslate(position);
	object_->RegistWorldTransform(&wt_);

	/* コライダーの適用処理 */
	// コライダーを生成
	collider_ = std::make_unique<ObjectCollider>(object_.get());

	// ObjectColliderにダウンキャストして詳細設定
	auto* chipCollider = dynamic_cast<ObjectCollider*>(collider_.get());
	if (chipCollider) {
		// 衝突属性を設定
		chipCollider->SetCollisionAttribute(attribute);
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
