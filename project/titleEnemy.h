#pragma once

#include "engine/3d/object/Object3d.h"
#include "engine/3d/worldTransform/WorldTransform.h"
#include "engine/3d/light/PointLight.h"

#include <memory>

class titleEnemy {
public:

	void Initialize(Norm::Vector3 _pos);

	void Update();

	void DebugWithImGui();

	Norm::PointLight* GetPointLight() const { return pointLight_.get(); }

	void SetDirection(Norm::Vector3 _direction) { direction_ = _direction; }

	void SetIsRun(bool const flag) { isRun_ = flag; }

private:

	std::unique_ptr<Norm::Object3d> object_;

	Norm::WorldTransform wt_;

	std::unique_ptr<Norm::PointLight> pointLight_;

	Norm::Vector3 direction_ = { 1.0f,0.0f,0.0f };

	float speed_ = 0.0f;

	float minSpeed_ = 0.05f;

	float maxSpeed_ = 0.1f;

	float frequency_ = 0.0f;

	float minFrequency_ = 2.0f;

	float maxFrequency_ = 3.0f;

	float maxLength_ = 45.0f;

	float timer_ = 0.0f;

	float startPosY_ = 0.0f;

	bool isRun_ = false;
};