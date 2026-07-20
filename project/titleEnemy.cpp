#include "titleEnemy.h"

#include "engine/3d/object/Object3dManager.h"
#include "engine/input/Input.h"
#include "engine/math/MyMath.h"

#include <random>

using namespace Norm;

void titleEnemy::Initialize(Vector3 _pos) {

	object_ = std::make_unique<Object3d>();
	object_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("titleEnemy"), "cube");
	wt_.Initialize();
	wt_.SetTranslate(_pos);
	object_->RegistWorldTransform(&wt_);

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->SetIntensity(0.5f);
	pointLight_->SetRadius(2.0f);
	pointLight_->SetDecay(1.5f);

	startPosY_ = wt_.GetTranslate().y;

	std::random_device seed_gen;
	std::uint32_t seed = seed_gen();
	std::mt19937 engine(seed);

	std::uniform_real_distribution<float> dist1(minSpeed_, maxSpeed_);

	std::uniform_real_distribution<float> dist2(minFrequency_, maxFrequency_);

	speed_ = dist1(engine);

	frequency_ = dist2(engine);
}

void titleEnemy::Update() {

	timer_ += 1.0f / 60.0f;

	Vector3 pos = wt_.GetTranslate();

	if (isRun_) {

		pos += MyMath::Normalize(pos);
	} else {

		pos.x += direction_.x * speed_;

		pos.y = startPosY_ + std::sinf(timer_ * frequency_) * 2.0f;
	}

	if (MyMath::Length(pos) >= maxLength_) {

		direction_.x = direction_.x * -1.0f;
	}

	wt_.SetTranslate(pos);

	wt_.UpdateMatrix();

	pointLight_->SetPosition(wt_.GetTranslate() + Vector3(0.0f, 1.0f, -1.0f));
}

void titleEnemy::DebugWithImGui() {

	//pointLight_->DebugWithImGui(L"エネミー点光源 1");
}