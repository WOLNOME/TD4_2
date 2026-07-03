#include "MapChip.h"

void Norm::MapChip::Initialize(Type type, const Vector3& position) { 
	type_ = type;
	object_ = std::make_unique<Object3d>();

	// タイプに応じてモデル名を切り替える予定
	std::string modelName = "normalBlock";

	object_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName(modelName), modelName);
	wt_.Initialize();
	wt_.SetTranslate(position);
	object_->RegistWorldTransform(&wt_);
}
