#include "LightManager.h"
#include <cassert>

//Engine
#include <Input.h>
#include <MyMath.h>
#include <PointLight.h>

using namespace Norm;

void LightManager::Initialize() {

}

void LightManager::Update() {
	assert(pointLight_ && "ポイントライトを設定してください");

	//左クリックでフラッシュ
	if (Input::GetInstance()->TriggerMouseButton(MouseButton::LeftButton)) {
		//輝度を最大まで上げる
		pointLight_->SetIntensity(kMaxIntensity);
		//範囲を最大まで上げる
		pointLight_->SetRadius(kMaxRange);
		//フラッシュフラグをオンにする
		isFlush_ = true;
	}

	//フラッシュ後に明るさと範囲を徐々に戻していく処理
	if (isFlush_) {
		flushKeepTimer_ += kDeltaTime;

		float intensity = MyMath::Lerp(kMaxIntensity, kNormalIntensity, MyMath::EaseInCirc(flushKeepTimer_ / kFlushKeepTime));
		pointLight_->SetIntensity(intensity);

		float range = MyMath::Lerp(kMaxRange, kNormalRange, MyMath::EaseInCirc(flushKeepTimer_ / kFlushKeepTime));
		pointLight_->SetRadius(range);

		//タイマーが規定時間に達したら
		if (flushKeepTimer_ > kFlushKeepTime) {
			flushKeepTimer_ = 0.0f;
			pointLight_->SetIntensity(kNormalIntensity);
			pointLight_->SetRadius(kNormalRange);
		}
	}

	//ライトの座標を移動させる処理

	//



}

void LightManager::Debug() {

}