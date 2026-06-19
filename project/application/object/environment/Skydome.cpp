#include "Skydome.h"
#include "TextureManager.h"
#include "Object3dManager.h"

using namespace Norm;

void Skydome::Initialize() {
	//変数の初期化
	textureHandle_ = TextureManager::GetInstance()->LoadTexture("skybox.dds");
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(ShapeTag{},Object3dManager::GetInstance()->GenerateName("SkyBox"),Shape::ShapeKind::kSkyBox);
	object3d_->SetTexture(textureHandle_);
	object3d_->SetIsLightProcess(false);
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	Vector3 pos = worldTransform_.GetTranslate();
	pos.y += 120.0f;
	worldTransform_.SetTranslate(pos);
	worldTransform_.SetScale({ 1500.0f,1500.0f,1500.0f });
	//オブジェクトにセット
	object3d_->RegistWorldTransform(&worldTransform_);

}
