#include "PauseMenu.h"

// Engine
#include <Input.h>
#include <TextureManager.h>
#include <Object3dManager.h>

void Norm::PauseMenu::Initialize() { 
	isPaused_ = false; 
	isRequestedReturnToTitle_ = false;

	/* スプライト生成 + 初期化 */

	// 背景（半透明）
	texBackground_ = TextureManager::GetInstance()->LoadTexture("white.png");
	spriteBackground_ = std::make_unique<Sprite>();
	spriteBackground_->Initialize(SpriteTag{}, Object3dManager::GetInstance()->GenerateName("Background"), Order::Front1, texBackground_);
	spriteBackground_->SetColor({0.0f, 0.0f, 0.0f, 0.75f});
	spriteBackground_->SetIsDisplay(false);

	// 「ポーズ」文字
	texTextPause_ = TextureManager::GetInstance()->LoadTexture("text_pause.png");
	spriteTextPause_ = std::make_unique<Sprite>();
	spriteTextPause_->Initialize(SpriteTag{}, Object3dManager::GetInstance()->GenerateName("TextPause"), Order::Front2, texTextPause_);
	spriteTextPause_->SetAnchorPoint({0.5f, 0.5f});
	spriteTextPause_->SetPosition(kTextPausePosition);
	spriteTextPause_->SetIsDisplay(false);
	spriteTextPause_->GetSize();

	// 「つづける」文字
	texTextContinue_ = TextureManager::GetInstance()->LoadTexture("text_continue.png");
	spriteTextContinue_ = std::make_unique<Sprite>();
	spriteTextContinue_->Initialize(SpriteTag{}, Object3dManager::GetInstance()->GenerateName("TextContinue"), Order::Front2, texTextContinue_);
	spriteTextContinue_->SetAnchorPoint({0.5f, 0.5f});
	spriteTextContinue_->SetPosition(kTextContinuePosition);
	spriteTextContinue_->SetIsDisplay(false);
	sizeContinueDefault_ = spriteTextContinue_->GetSize();

	// 「タイトルへ戻る」文字
	texTextReturnToTitle_ = TextureManager::GetInstance()->LoadTexture("text_returnToTitle.png");
	spriteTextReturnToTitle_ = std::make_unique<Sprite>();
	spriteTextReturnToTitle_->Initialize(SpriteTag{}, Object3dManager::GetInstance()->GenerateName("TextReturnToTitle"), Order::Front2, texTextReturnToTitle_);
	spriteTextReturnToTitle_->SetAnchorPoint({0.5f, 0.5f});
	spriteTextReturnToTitle_->SetPosition(kTextReturnToTitlePosition);
	spriteTextReturnToTitle_->SetIsDisplay(false);
	sizeReturnToTitleDefault_ = spriteTextReturnToTitle_->GetSize();

	// ESCキー
	texKeyEscape_ = TextureManager::GetInstance()->LoadTexture("key_escape.png");
	spriteKeyEscape_ = std::make_unique<Sprite>();
	spriteKeyEscape_->Initialize(SpriteTag{}, Object3dManager::GetInstance()->GenerateName("TextReturnToTitle"), Order::Front0, texKeyEscape_);
	spriteKeyEscape_->SetAnchorPoint({0.5f, 0.5f});
	spriteKeyEscape_->SetPosition({60.0f, 60.0f});
	spriteKeyEscape_->SetIsDisplay(true);

	// 音声読み込み
	seClick_ = std::make_unique<Norm::Audio>();
	seClick_->Initialize("click.wav");

	sePause_ = std::make_unique<Norm::Audio>();
	sePause_->Initialize("pause.wav");
}

void Norm::PauseMenu::Update() { 
	Input* input = Input::GetInstance();

	// ESCキーが押されたらポーズ状態を切り替える
	if (input->TriggerKey(DIK_ESCAPE)) {
		// ポーズキー押下音再生
		sePause_->Play(false, 0.4f);

		isPaused_ = !isPaused_;

		// 各スプライトの表示状態を切り替える
		spriteBackground_->SetIsDisplay(isPaused_);
		spriteTextPause_->SetIsDisplay(isPaused_);
		spriteTextContinue_->SetIsDisplay(isPaused_);
		spriteTextReturnToTitle_->SetIsDisplay(isPaused_);

		// サイズと色を元に戻す
		if (!isPaused_) {
			spriteTextContinue_->SetSize(sizeContinueDefault_);
			spriteTextContinue_->SetColor(kColorWhite);
			spriteTextReturnToTitle_->SetSize(sizeReturnToTitleDefault_);
			spriteTextReturnToTitle_->SetColor(kColorWhite);
		}
	}

	// ポーズ中のみマウス入力とホバーチェックを処理する
	if (isPaused_) {
		// マウス座標を取得
		Vector2 mousePos = input->GetMousePosition();

		// 「つづける」ボタン
		if (IsMouseOverSprite(spriteTextContinue_.get(), mousePos)) {
			spriteTextContinue_->SetColor(kColorRed); // 範囲内なら赤くする
			spriteTextContinue_->SetSize({sizeContinueDefault_.x * 1.1f, sizeContinueDefault_.y * 1.1f}); // 1.1倍にする

			// 左クリックされたらポーズ解除
			if (input->TriggerMouseButton(MouseButton::LeftButton)) {
				// クリック音再生
				seClick_->Play(false, 0.5f);

				isPaused_ = false;

				// スプライトを非表示に戻す
				spriteBackground_->SetIsDisplay(false);
				spriteTextPause_->SetIsDisplay(false);
				spriteTextContinue_->SetIsDisplay(false);
				spriteTextReturnToTitle_->SetIsDisplay(false);
			}
		} else {
			spriteTextContinue_->SetColor(kColorWhite); // 範囲外なら白に戻す
			spriteTextContinue_->SetSize(sizeContinueDefault_); // 等倍に戻す
		}

		// 「タイトルへ戻る」ボタン
		if (IsMouseOverSprite(spriteTextReturnToTitle_.get(), mousePos)) {
			spriteTextReturnToTitle_->SetColor(kColorRed); // 範囲内なら赤くする
			spriteTextReturnToTitle_->SetSize({sizeReturnToTitleDefault_.x * 1.1f, sizeReturnToTitleDefault_.y * 1.1f}); // 1.1倍にする

			// 左クリックされたらリクエストフラグを立てる
			if (input->TriggerMouseButton(MouseButton::LeftButton)) {
				// クリック音再生
				seClick_->Play(false, 0.5f);

				isRequestedReturnToTitle_ = true;
			} 
		} else {
			spriteTextReturnToTitle_->SetColor(kColorWhite); // 範囲外なら白に戻す
			spriteTextReturnToTitle_->SetSize(sizeReturnToTitleDefault_); // 等倍に戻す
		}
	}
}

bool Norm::PauseMenu::IsMouseOverSprite(const Sprite* sprite, const Vector2& mousePos) {
	if (!sprite) return false;

	Vector2 pos = sprite->GetPosition();
	Vector2 size = sprite->GetSize();

	// アンカーポイントを中心にしているので調整
	float left = pos.x - (size.x * 0.5f);
	float right = pos.x + (size.x * 0.5f);
	float top = pos.y - (size.y * 0.5f);
	float bottom = pos.y + (size.y * 0.5f);

	// マウスがこの座標に入っているか
	return (mousePos.x >= left && mousePos.x <= right && mousePos.y >= top && mousePos.y <= bottom);
}
