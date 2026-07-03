#include "GuideUI.h"

using namespace Norm;

#include "engine/base/windows/WinApp.h"
#include "engine/base/texture/TextureManager.h"
#include "engine/2d/SpriteManager.h"
#include "engine/math/MyMath.h"

#include "imgui.h"

#include <algorithm>

void GuideUI::Initialize(BaseCamera* _camera, Norm::Input* _input) {

	camera_ = _camera;
	input_ = _input;

	textureHandle_ = TextureManager::GetInstance()->LoadTexture("mouse.png");
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("testUI"), Order::Front2, textureHandle_);
	sprite_->SetAnchorPoint({ 0.5f,0.5f });
	sprite_->SetPosition({ 0.0f,0.0f });

	worldTransform_.Initialize();
	worldTransform_.SetTranslate({ 0.0f,13.0f,40.0f });
}

void GuideUI::Update() {

	worldTransform_.UpdateMatrix();

	Vector3 translate = worldTransform_.GetTranslate();

	Matrix4x4 viewport = MyMath::MakeViewportMatrix(0, 0, static_cast<float>(WinApp::GetInstance()->kClientWidth), static_cast<float>(WinApp::GetInstance()->kClientHeight), 0, 1);

	Matrix4x4 viewProjection = camera_->GetViewProjectionMatrix();

	Matrix4x4 viewProjectionViewport = viewProjection * viewport;

	//3Dオブジェクトの座標をスクリーン座標に変換する
	Vector3 screenPos = MyMath::Transform(translate, viewProjectionViewport);

	sprite_->SetPosition({ screenPos.x,screenPos.y });
	//ベクトル1を求める
	Vector3 cameraPos = camera_->worldTransform.GetWorldTranslate();
	Vector3 pointX;	//マウスのスクリーン座標をワールド座標に変換したときのある点
	Vector3 mousePos = { input_->GetMousePosition().x,input_->GetMousePosition().y,0.0f };
	float ndcX = (2.0f * mousePos.x / WinApp::GetInstance()->kClientWidth) - 1.0f;
	float ndcY = 1.0f - (2.0f * mousePos.y / WinApp::GetInstance()->kClientHeight);
	Vector3 pointNDC =
	{
		ndcX,
		ndcY,
		1.0f
	};
	Matrix4x4 invViewProj =
		MyMath::Inverse(camera_->GetViewProjectionMatrix());
	pointX = MyMath::Transform(pointNDC, invViewProj);
	//ベクトル1を直線に変換
	Line line;
	line.diff = Vector3(pointX - cameraPos).Normalized();
	line.origin = cameraPos;
	//YZ平面を作成
	Plane YZPlane;
	YZPlane.normal = { 1,0,0 };
	YZPlane.distance = 0.0f;
	//直線と平面の交点CPを求める
	Vector3 cp = MyMath::CollisionPoint(line, YZPlane);

	distance_ = MyMath::Length(translate - cp);

	if (distance_ <= acceptableLange_) {

		alpha_ += alphaSpeed_;
	} else {

		alpha_ -= alphaSpeed_;
	}

	alpha_ = std::clamp(alpha_, 0.0f, 1.0f);

	sprite_->SetColor(Vector4(spriteColor_.x, spriteColor_.y, spriteColor_.z, alpha_));
}

void GuideUI::ImGui() {

#ifdef _DEBUG

	Vector3 translate = worldTransform_.GetTranslate();

	Vector2 mousePos = input_->GetMousePosition();

	ImGui::Begin("操作UI");

	ImGui::DragFloat3("ワールド座標", &translate.x, 0.1f);

	ImGui::DragFloat2("マウス座標", &mousePos.x);

	ImGui::DragFloat("マウス間の距離", &distance_);

	ImGui::ColorEdit4("スプライト色", &spriteColor_.x);

	ImGui::End();

#endif // _DEBUG

	worldTransform_.SetTranslate(translate);
}