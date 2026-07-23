#pragma once
#include "engine/3d/camera/BaseCamera.h"
#include "engine/input/Input.h"
#include "application/object/Character/Player.h"
#include "application/ui/flag/FlagUI.h"

#include <memory>
#include <vector>

class OperationUI {

public:

	void Initialize(Norm::Player* _player, Norm::BaseCamera* _camera, Norm::Input* _input);

	void Update(const bool _isPause);

private:

	Norm::Player* player_;

	Norm::BaseCamera* camera_;

	Norm::Input* input_;

	std::unique_ptr<FlagUI> wKeyUI_;

	std::unique_ptr<FlagUI> aKeyUI_;

	std::unique_ptr<FlagUI> sKeyUI_;

	std::unique_ptr<FlagUI> dKeyUI_;

	std::unique_ptr<FlagUI> leftMouseUI_;

	Norm::Vector3 keyUIPos_ = { 3.5f, 1.0f, 0.0f };

	float keyUIDistance_ = 0.9f;

	Norm::Vector2 keyUISize_ = { 32.0f,32.0f };

	Norm::Vector3 mouseUIPos_ = { 3.5f,-1.0f,0.0f };

	Norm::Vector2 mouseUISize_ = { 64.0f,64.0f };

	int32_t hpTexture_;

	std::vector<std::unique_ptr<Norm::Sprite>> hpUI_;

	Norm::Vector3 hpUIOffset_ = { -2.75f,1.0f,0.0f };

	float hpUISizeX_ = 1.25f;

	int playerMaxHP_ = 0;

	int32_t moveHelpTexture_;

	int32_t flashHelpTexture_;

	std::unique_ptr<Norm::Sprite> moveHelpUI_;

	std::unique_ptr<Norm::Sprite> flashHelpUI_;

	Norm::Vector3 moveHelpOffset_ = { 4.75f, 1.5f, 0.0f };

	Norm::Vector3 flashHelpOffset_ = { 4.75f,-1.0f, 0.0f };

	Norm::Vector4 uiColor_ = { 0.5f,0.5f,0.5f,1.0f };

};