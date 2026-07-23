#include "OperationUI.h"

#include "engine/base/texture/TextureManager.h"
#include "engine/2d/SpriteManager.h"
#include "application/ui/math/UIMath.h"

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
		Vector3(3.25f, 2.25f, 0.0f),
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
		Vector3(2.0f, 1.0f, 0.0f),
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
		Vector3(3.25f, 1.0f, 0.0f),
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
		Vector3(4.5f, 1.0f, 0.0f),
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
		Vector3(3.25f, -1.0f, 0.0f),
		DIK_0,
		true,
		camera_,
		input_
	);

	hpTexture_ = TextureManager::GetInstance()->LoadTexture("whiteHeart.png");

	for (int i = 0; i < player_->GetHP(); i++) {

		std::unique_ptr<Sprite> newUI = std::make_unique<Sprite>();
		newUI->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("hpUI"), Order::Front2, hpTexture_);
		newUI->SetAnchorPoint({ 0.5f,0.5f });
		newUI->SetPosition({ 0.0f,0.0f });
		newUI->SetSize({ 48.0f,48.0f });

		hpUI_.push_back(std::move(newUI));
	}

	playerMaxHP_ = player_->GetHP();

	moveHelpTexture_ = TextureManager::GetInstance()->LoadTexture("moveHelp.png");
	moveHelpUI_ = std::make_unique<Sprite>();
	moveHelpUI_->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("moveHelpUI"), Order::Front2, moveHelpTexture_);
	moveHelpUI_->SetAnchorPoint({ 0.5f,0.5f });
	moveHelpUI_->SetPosition({ 0.0f,0.0f });

	flashHelpTexture_ = TextureManager::GetInstance()->LoadTexture("flashHelp.png");
	flashHelpUI_ = std::make_unique<Sprite>();
	flashHelpUI_->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("flashHelpUI"), Order::Front2, flashHelpTexture_);
	flashHelpUI_->SetAnchorPoint({ 0.5f,0.5f });
	flashHelpUI_->SetPosition({ 0.0f,0.0f });

}

void OperationUI::Update() {

	Vector2 screenPos = WorldToScreen(player_->GetTranslate(), camera_->GetViewProjectionMatrix());

	wKeyUI_->Update(player_->GetTranslate());
	aKeyUI_->Update(player_->GetTranslate());
	sKeyUI_->Update(player_->GetTranslate());
	dKeyUI_->Update(player_->GetTranslate());
	leftMouseUI_->Update(player_->GetTranslate());

	int hpCount = player_->GetHP();

	int maxCount = playerMaxHP_;

	for (auto& ui : hpUI_) {

		if (hpCount > 0) {

			ui->SetColor({ 1.0f,1.0f,1.0f,1.0f });

		} else {

			ui->SetColor({ 0.05f,0.05f,0.05f,1.0f });

			hpCount = 0;
		}

		Vector3 offset = hpUIOffset_ - hpUISizeX_ * (playerMaxHP_ - maxCount);

		ui->SetPosition(WorldToScreen(player_->GetTranslate() + offset, camera_->GetViewProjectionMatrix()));

		hpCount--;

		maxCount--;
	}

	moveHelpUI_->SetPosition(WorldToScreen(player_->GetTranslate() + moveHelpOffset_, camera_->GetViewProjectionMatrix()));
	flashHelpUI_->SetPosition(WorldToScreen(player_->GetTranslate() + flashHelpOffset_, camera_->GetViewProjectionMatrix()));
}