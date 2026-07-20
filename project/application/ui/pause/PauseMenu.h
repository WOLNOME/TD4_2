#pragma once

// Engine
#include <MyMath.h>
#include <Sprite.h>
#include <Audio.h>

namespace Norm {
// =========================================================
// ポーズメニュークラス
// =========================================================
class PauseMenu {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	// =========================================================
	// Accessor
	// =========================================================

	/// <summary>
	/// ポーズ中かどうかを取得
	/// </summary>
	/// <returns></returns>
	bool IsPaused() const { return isPaused_; }

	/// <summary>
	/// タイトルへ戻るが押されたかを取得
	/// </summary>
	/// <returns></returns>
	bool IsRequestedReturnToTitle() const { return isRequestedReturnToTitle_; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// マウスがスプライト内にあるか判定
	/// </summary>
	/// <param name="sprite"></param>
	/// <param name="mousePos"></param>
	/// <returns></returns>
	bool IsMouseOverSprite(const class Sprite* sprite, const Vector2& mousePos);

private:
	// =========================================================
	// Constants
	// =========================================================

	const Vector2 kTextPausePosition = {640.0f, 220.0f};
	const Vector2 kTextContinuePosition = {640.0f, 360.0f};
	const Vector2 kTextReturnToTitlePosition = {640.0f, 460.0f};

	const Vector4 kColorWhite = {1.0f, 1.0f, 1.0f, 1.0f};
	const Vector4 kColorRed = {1.0f, 0.0f, 0.0f, 1.0f};

	// =========================================================
	// Member Variables
	// =========================================================

	// ポーズ状態フラグ
	bool isPaused_ = false;
	// タイトルへ戻るが押されたフラグ
	bool isRequestedReturnToTitle_ = false;

	// ボタンの元サイズを保存
	Vector2 sizeContinueDefault_{};
	Vector2 sizeReturnToTitleDefault_{};

	/* スプライト */
	// 背景
	uint32_t texBackground_ = 0u;
	std::unique_ptr<Sprite> spriteBackground_ = nullptr;
	// 「ポーズ」文字
	uint32_t texTextPause_ = 0u;
	std::unique_ptr<Sprite> spriteTextPause_ = nullptr;
	// 「つづける」文字
	uint32_t texTextContinue_ = 0u;
	std::unique_ptr<Sprite> spriteTextContinue_ = nullptr;
	// 「タイトルへ戻る」文字
	uint32_t texTextReturnToTitle_ = 0u;
	std::unique_ptr<Sprite> spriteTextReturnToTitle_ = nullptr;
	// ESCキー
	uint32_t texKeyEscape_ = 0u;
	std::unique_ptr<Sprite> spriteKeyEscape_ = nullptr;

	// クリック音
	std::unique_ptr<Norm::Audio> seClick_;
	// ポーズキー押下音
	std::unique_ptr<Norm::Audio> sePause_;
};
} // namespace Norm
