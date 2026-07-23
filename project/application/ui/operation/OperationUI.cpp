#include "OperationUI.h"

using namespace Norm;

void OperationUI::Initialize(Norm::Player* _player, Norm::BaseCamera* _camera, Norm::Input* _input) {

	player_ = _player;

	camera_ = _camera;

	input_ = _input;


	wKeyUI_ = std::make_unique<FlagUI>();
	wKeyUI_->Initialize(
		"wKeyUI",
		"keyboard_w.png",
		"keyboard_w_outline.png",
		Vector3(-3.0f, 2.25f, 0.0f),
		DIK_W,
		false,
		camera_,
		input_
	);

	aKeyUI_ = std::make_unique<FlagUI>();
	aKeyUI_->Initialize(
		"aKeyUI",
		"keyboard_a.png",
		"keyboard_a_outline.png",
		Vector3(-4.25f, 1.0f, 0.0f),
		DIK_A,
		false,
		camera_,
		input_
	);

	sKeyUI_ = std::make_unique<FlagUI>();
	sKeyUI_->Initialize(
		"sKeyUI",
		"keyboard_s.png",
		"keyboard_s_outline.png",
		Vector3(-3.0f, 1.0f, 0.0f),
		DIK_S,
		false,
		camera_,
		input_
	);

	dKeyUI_ = std::make_unique<FlagUI>();
	dKeyUI_->Initialize(
		"dKeyUI",
		"keyboard_d.png",
		"keyboard_d_outline.png",
		Vector3(-1.75f, 1.0f, 0.0f),
		DIK_D,
		false,
		camera_,
		input_
	);

	leftMouseUI_ = std::make_unique<FlagUI>();
	leftMouseUI_->Initialize(
		"leftMouseUI",
		"mouse_left_outline.png",
		"mouse_outline.png",
		Vector3(-3.0f, -1.0f, 0.0f),
		DIK_0,
		true,
		camera_,
		input_
	);

}

void OperationUI::Update() {

	wKeyUI_->Update(player_->GetTranslate());
	aKeyUI_->Update(player_->GetTranslate());
	sKeyUI_->Update(player_->GetTranslate());
	dKeyUI_->Update(player_->GetTranslate());
	leftMouseUI_->Update(player_->GetTranslate());
}