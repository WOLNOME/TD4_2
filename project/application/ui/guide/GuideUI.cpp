#include "GuideUI.h"

using namespace Norm;

#include "engine/base/windows/WinApp.h"
#include "engine/base/texture/TextureManager.h"
#include "engine/2d/SpriteManager.h"
#include "engine/math/MyMath.h"

void GuideUI::Initialize(BaseCamera* _camera) {

	textureHandle_ = TextureManager::GetInstance()->LoadTexture("mouse.png");
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("testUI"), Order::Front2, textureHandle_);
	sprite_->SetAnchorPoint({ 0.5f,0.5f });
	sprite_->SetPosition({ 0.0f,0.0f });

	worldTransform_.Initialize();
	worldTransform_.SetTranslate({ 1.0f,1.0f,20.0f });

	camera_ = _camera;
}

void GuideUI::Update() {

	worldTransform_.UpdateMatrix();

	Vector3 transform = worldTransform_.GetTranslate();

	Matrix4x4 viewport = MyMath::MakeViewportMatrix(0, 0, static_cast<float>(WinApp::GetInstance()->kClientWidth), static_cast<float>(WinApp::GetInstance()->kClientHeight), 0, 1);

	Matrix4x4 viewProjection = camera_->GetViewProjectionMatrix();

	Matrix4x4 viewProjectionViewport = viewProjection * viewport;

	//3Dオブジェクトの座標をスクリーン座標に変換する
	Vector3 screenPos = MyMath::Transform(transform, viewProjectionViewport);

	sprite_->SetPosition({ screenPos.x,screenPos.y });
}