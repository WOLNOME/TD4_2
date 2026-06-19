#include "Ground.h"
#include "TextureManager.h"
#include "Object3dManager.h"

using namespace Norm;

void Ground::Initialize() {
	//オブジェクトの生成・初期化
	textureHandle_ = TextureManager::GetInstance()->LoadTexture("ground.png");
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(ModelTag{},Object3dManager::GetInstance()->GenerateName("Ground"), "ground");
	object3d_->SetTexture(textureHandle_);
	object3d_->SetIsLightProcess(false);
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.SetScale({ 3.0f, 1.0f, 3.0f });
	//オブジェクトにセット
	object3d_->RegistWorldTransform(&worldTransform_);

}
