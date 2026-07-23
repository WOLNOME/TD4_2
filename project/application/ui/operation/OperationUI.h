#pragma once
#include "engine/3d/camera/BaseCamera.h"
#include "engine/input/Input.h"
#include "application/object/Character/Player.h"
#include "application/ui/flag/FlagUI.h"

class OperationUI {

public:

	void Initialize(Norm::Player* _player, Norm::BaseCamera* _camera, Norm::Input* _input);

	void Update();

private:

	Norm::Player* player_;

	Norm::BaseCamera* camera_;

	Norm::Input* input_;

	std::unique_ptr<FlagUI> wKeyUI_;
	std::unique_ptr<FlagUI> aKeyUI_;
	std::unique_ptr<FlagUI> sKeyUI_;
	std::unique_ptr<FlagUI> dKeyUI_;
	std::unique_ptr<FlagUI> leftMouseUI_;

};