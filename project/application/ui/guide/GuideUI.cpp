#include "GuideUI.h"

using namespace Norm;

#include "engine/base/windows/WinApp.h"
#include "engine/base/texture/TextureManager.h"
#include "engine/2d/SpriteManager.h"
#include "engine/math/MyMath.h"

#include "imgui.h"

#include <algorithm>

void GuideUI::Initialize(BaseCamera* _camera, Input* _input, Vector3 _pos) {

	camera_ = _camera;
	input_ = _input;

	mouseTextureHandle_ = TextureManager::GetInstance()->LoadTexture("mouse_outline.png");
	clickMouseTextureHandle_ = TextureManager::GetInstance()->LoadTexture("mouse_left_outline.png");
	clickTextureHandle_ = TextureManager::GetInstance()->LoadTexture("click.png");

	mouseSprite_ = std::make_unique<Sprite>();
	mouseSprite_->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("guideMouseUI"), Order::Front2, mouseTextureHandle_);
	mouseSprite_->SetAnchorPoint({ 0.5f,0.5f });
	mouseSprite_->SetPosition({ 0.0f,0.0f });

	mouseWorldTransform_.Initialize();
	mouseWorldTransform_.SetTranslate(_pos);

	clickSprite_ = std::make_unique<Sprite>();
	clickSprite_->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("guideClickUI"), Order::Front2, clickTextureHandle_);
	clickSprite_->SetAnchorPoint({ 0.5f,0.5f });
	clickSprite_->SetPosition({ 0.0f,0.0f });
}

void GuideUI::Update() {

	textureChangeTimer_ += 1.0f / 60.0f;

	UpdateSpritePos();

	mouseWorldTransform_.UpdateMatrix();

	if (GetDistanceToMouse() <= acceptableLange_) {

		alpha_ += alphaSpeed_;
	} else {

		alpha_ -= alphaSpeed_;
	}

	alpha_ = std::clamp(alpha_, 0.0f, 1.0f);

	mouseSprite_->SetColor(Vector4(spriteColor_.x, spriteColor_.y, spriteColor_.z, alpha_));

	if (textureChangeTimer_ >= textureChangeMaxTime_) {

		isClickTexture_ = !isClickTexture_;

		textureChangeTimer_ = 0.0f;
	}

	if (isClickTexture_) {

		mouseSprite_->SetTexture(clickMouseTextureHandle_);

		clickSprite_->SetColor(Vector4(spriteColor_.x, spriteColor_.y, spriteColor_.z, alpha_));
	} else {

		mouseSprite_->SetTexture(mouseTextureHandle_);

		clickSprite_->SetColor(Vector4(0.0f,0.0f,0.0f,0.0f));
	}

}

void GuideUI::ImGui() {

#ifdef _DEBUG

	Vector3 translate = mouseWorldTransform_.GetTranslate();

	Vector2 mousePos = input_->GetMousePosition();

	ImGui::Begin("操作UI");

	ImGui::DragFloat3("ワールド座標", &translate.x, 0.1f);

	ImGui::ColorEdit4("スプライト色", &spriteColor_.x);

	ImGui::End();

	mouseWorldTransform_.SetTranslate(translate);

#endif // _DEBUG

	
}

void GuideUI::UpdateSpritePos() {

	Vector3 translate = mouseWorldTransform_.GetTranslate();

	Matrix4x4 viewport = MyMath::MakeViewportMatrix(0, 0, static_cast<float>(WinApp::GetInstance()->kClientWidth), static_cast<float>(WinApp::GetInstance()->kClientHeight), 0, 1);

	Matrix4x4 viewProjection = camera_->GetViewProjectionMatrix();

	Matrix4x4 viewProjectionViewport = viewProjection * viewport;

	//3Dオブジェクトの座標をスクリーン座標に変換する
	Vector3 screenPos = MyMath::Transform(translate, viewProjectionViewport);

	mouseSprite_->SetPosition({ screenPos.x,screenPos.y });

	clickSprite_->SetPosition({ screenPos.x + clickSpriteOffset_.x,screenPos.y + clickSpriteOffset_.y });
}

float GuideUI::GetDistanceToMouse() {

	Vector3 translate = mouseWorldTransform_.GetTranslate();

	Vector3 cameraPos = camera_->worldTransform.GetWorldTranslate();

	Vector3 mousePos = { input_->GetMousePosition().x,input_->GetMousePosition().y,0.0f };

	Matrix4x4 invViewProjection = MyMath::Inverse(camera_->GetViewProjectionMatrix());

	//マウス座標を正規化デバイス座標に変換
	float ndcX = (2.0f * mousePos.x / WinApp::GetInstance()->kClientWidth) - 1.0f;
	float ndcY = 1.0f - (2.0f * mousePos.y / WinApp::GetInstance()->kClientHeight);

	Vector3 pointNDC = { ndcX,ndcY,1.0f };

	//正規化デバイス座標をワールド座標に変換
	Vector3 mouseWorldPos = MyMath::Transform(pointNDC, invViewProjection);

	//カメラ座標からマウス座標への直線を生成
	Line line;
	line.diff = Vector3(mouseWorldPos - cameraPos).Normalized();
	line.origin = cameraPos;

	//XY平面を作成
	Plane YZPlane;
	YZPlane.normal = { 0,0,1 };
	YZPlane.distance = 0.0f;

	//直線と平面の交点を求める
	Vector3 closestPoint= MyMath::CollisionPoint(line, YZPlane);

	return MyMath::Length(translate - closestPoint);
}
