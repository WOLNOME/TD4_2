#include "TitleEnemy.h"

#include "engine/3d/object/Object3dManager.h"
#include "engine/input/Input.h"
#include "engine/math/MyMath.h"

#include <random>
#include <numbers>

using namespace Norm;

void TitleEnemy::Initialize(Vector3 _pos) {

	//オブジェクトの生成
	object_ = std::make_unique<Object3d>();
	object_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("titleEnemy"), "ghost");

	//ワールドトランスフォームの初期化
	wt_.Initialize();
	wt_.SetTranslate(_pos + Vector3(0.0f, 0.0f, 1.0f));
	wt_.SetScale(Vector3(2.0f, 2.0f, 2.0f));

	//オブジェクトにワールドトランスフォームを設定
	object_->RegistWorldTransform(&wt_);

	//ライトの設定
	pointLight_ = std::make_unique<PointLight>();
	pointLight_->SetIntensity(0.5f);
	pointLight_->SetRadius(2.0f);
	pointLight_->SetDecay(1.5f);

	//初期Y座標の取得
	startPosY_ = wt_.GetTranslate().y;

	//ランダム値の設定
	std::random_device seed_gen;
	std::uint32_t seed = seed_gen();
	std::mt19937 engine(seed);

	std::uniform_real_distribution<float> dist1(minSpeed_, maxSpeed_);

	std::uniform_real_distribution<float> dist2(minFrequency_, maxFrequency_);

	speed_ = dist1(engine);

	frequency_ = dist2(engine);
}

void TitleEnemy::Update() {

	timer_ += 1.0f / 60.0f;

	//座標の更新
	Vector3 pos = wt_.GetTranslate();

	Vector3 velocity = { 0.0f,0.0f,0.0f };

	if (isRun_) {

		//中心から離れるように移動させる
		velocity = MyMath::Normalize(pos);
	} else {

		velocity.x += direction_.x * speed_;

		//上下に反復移動させる
		pos.y = startPosY_ + std::sinf(timer_ * frequency_) * 2.0f;
	}

	pos += velocity;

	if (MyMath::Length(pos) >= maxLength_) {

		//範囲外に出たら反転
		direction_.x = direction_.x * -1.0f;
	}

	//
	float pi = std::numbers::pi_v<float> / 2.0f;

	//座標の設定
	wt_.SetTranslate(pos);

	//向きを移動方向に合わせる
	wt_.SetRotate({ 0.0f, -pi + (pi * direction_.x),0.0f });

	wt_.UpdateMatrix();

	pointLight_->SetPosition(wt_.GetTranslate() + Vector3(0.0f, 1.0f, -1.0f));
}

void TitleEnemy::DebugWithImGui() {

}