#include "LightManager.h"
#include <cassert>

//Engine
#include <Input.h>
#include <MyMath.h>
#include <PointLight.h>
#include <BaseCamera.h>
#include <WinApp.h>

using namespace Norm;

void LightManager::Initialize() {
}

void LightManager::Update() {
	isFlashTriggered_ = false;

	assert(pointLight_ && "ポイントライトを設定してください");
	assert(camera_ && "カメラを設定してください");

	//クールタイムの計算
	if (flushCoolTimer_ > 0.0f) {
		flushCoolTimer_ -= kDeltaTime;
		if (flushCoolTimer_ <= 0.0f) {
			flushCoolTimer_ = 0.0f;
		}
	}

	//左クリックでフラッシュ
	if (Input::GetInstance()->TriggerMouseButton(MouseButton::LeftButton) && !isFlush_ && flushCoolTimer_ == 0.0f) {
		//輝度を最大まで上げる
		pointLight_->SetIntensity(kMaxIntensity);
		//範囲を最大まで上げる
		pointLight_->SetRadius(kMaxRange);
		//フラッシュフラグをオンにする
		isFlush_ = true;
		isFlashTriggered_ = true;
		//クールタイマーをセット
		flushCoolTimer_ = kFlushCoolTime;
	}

	//フラッシュ後に明るさと範囲を徐々に戻していく処理
	if (isFlush_) {
		//タイマーを進める
		flushKeepTimer_ += kDeltaTime;

		//輝度と範囲を徐々にしぼめる
		float intensity = MyMath::Lerp(kMaxIntensity, kNormalIntensity, MyMath::EaseInSine(flushKeepTimer_ / kFlushKeepTime));
		pointLight_->SetIntensity(intensity);

		float range = MyMath::Lerp(kMaxRange, kNormalRange, MyMath::EaseInSine(flushKeepTimer_ / kFlushKeepTime));
		pointLight_->SetRadius(range);

		//タイマーが規定時間に達したら
		if (flushKeepTimer_ > kFlushKeepTime) {
			//タイマーをリセット
			flushKeepTimer_ = 0.0f;
			//輝度と範囲を通常に戻す
			pointLight_->SetIntensity(kNormalIntensity);
			pointLight_->SetRadius(kNormalRange);
			//フラッシュフラグをオフにする
			isFlush_ = false;
		}
	}

	//ライトをマウスで動かす処理
	{
		//インプットの取得
		auto* input = Input::GetInstance();

		//ベクトル1を求める
		Vector3 cameraPos = camera_->worldTransform.GetWorldTranslate();
		Vector3 pointX;	//マウスのスクリーン座標をワールド座標に変換したときのある点
		Vector3 mousePos = { input->GetMousePosition().x,input->GetMousePosition().y,0.0f };
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
		//XY平面を作成
		Plane XYPlane;
		XYPlane.normal = { 0,0,1 };
		XYPlane.distance = -2.0f;
		//直線と平面の交点CPを求める
		Vector3 cp = MyMath::CollisionPoint(line, XYPlane);
		//点光源の座標としてcpを適用する
		pointLight_->SetPosition(cp);
	}
}

void LightManager::Debug() {

}