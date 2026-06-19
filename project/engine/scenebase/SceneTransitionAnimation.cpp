#include "SceneTransitionAnimation.h"
#include "WinApp.h"
#include <SpriteManager.h>
#include  <TextureManager.h>
#include <cassert>
#include <algorithm>
#include <random>

namespace Norm {

	SceneTransitionAnimation::SceneTransitionAnimation()
		: state_(State::NONE), inType_(Type::NONE), outType_(Type::NONE), option_(Option::NONE), time_(0), timer_(0) {
	}

	SceneTransitionAnimation::~SceneTransitionAnimation() {
	}

	void SceneTransitionAnimation::Initialize() {
		//スプライト生成
		sprite_ = std::make_unique<Sprite>(true);
		uint32_t textureHandle = TextureManager::GetInstance()->LoadTexture("black.png");
		sprite_->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("transitionBack"), Order::SceneTransition, textureHandle);
		sprite_->SetIsDisplay(false);
		sprite_->SetAnchorPoint({ 0.5f,0.5f });

		//変数の初期化
		state_ = State::NONE;
		inType_ = Type::NONE;
		outType_ = Type::NONE;
		option_ = Option::NONE;
		time_ = 0.0f;
		timer_ = 0.0f;
	}

	void SceneTransitionAnimation::Update() {
	}

	void SceneTransitionAnimation::StartTransition() {
		//遷移中なら通らない
		if (state_ != State::NONE) {
			return;
		}
		//必要な変数の確認
		if (inType_ == Type::NONE || outType_ == Type::NONE) {
			assert(0 && "遷移の種類が設定されていません");
		}
		if (time_ == 0) {
			assert(0 && "時間が設定されていません");
		}

		//イン開始
		state_ = State::UPDATE_IN;
		//スプライトを表示
		sprite_->SetIsDisplay(true);
		//遷移中フラグを立てる
		isTransitioning_ = true;

		//オプション毎の設定
		switch (option_) {
		case SceneTransitionAnimation::Option::NONE:
			//特に何もしない
			break;
		case SceneTransitionAnimation::Option::SHAKE:
			//スプライトサイズを縦横+20大きくする
			sprite_->SetSize({ 1300.0f,740.0f });
			break;
		default:
			break;
		}

		//タイプ毎の設定
		switch (inType_) {
		case SceneTransitionAnimation::Type::FADE:
			//ポジションを画面中央に
			sprite_->SetPosition({ WinApp::kClientWidth / 2.0f,WinApp::kClientHeight / 2.0f });
			break;
		case SceneTransitionAnimation::Type::SLIDEUP:
			//ポジションを画面下に
			sprite_->SetPosition({ WinApp::kClientWidth / 2.0f,WinApp::kClientHeight + (sprite_->GetSize().y / 2.0f) });
			break;
		case SceneTransitionAnimation::Type::SLIDEDOWN:
			//ポジションを画面上に
			sprite_->SetPosition({ WinApp::kClientWidth / 2.0f,-(sprite_->GetSize().y / 2.0f) });
			break;
		default:
			break;
		}

	}

	void SceneTransitionAnimation::UpdateIn() {
		//状態がUPDATE_INでなければ通らない
		if (state_ != State::UPDATE_IN) {
			return;
		}

		//時間が達したら終了
		if (timer_ >= time_) {
			//フェードイン終了
			state_ = State::END_IN;
			return;
		}
		else {
			//時間を進める
			timer_ += kDeltaTime;
		}

		switch (inType_) {
		case SceneTransitionAnimation::Type::FADE:
		{
			//透明度を計算
			float alpha = MyMath::Lerp(0.0f, 1.0f, static_cast<float>(timer_) / static_cast<float>(time_));
			//スプライトの透明度を設定
			Vector4 color = sprite_->GetColor();
			color.w = alpha;
			sprite_->SetColor(color);
		}
		break;
		case SceneTransitionAnimation::Type::SLIDEUP:
		{
			//ポジションを計算
			float posY = MyMath::Lerp(WinApp::kClientHeight + (sprite_->GetSize().y / 2.0f), WinApp::kClientHeight / 2.0f, static_cast<float>(timer_) / static_cast<float>(time_));
			//クランプ
			posY = std::clamp(posY, WinApp::kClientHeight / 2.0f, WinApp::kClientHeight + (sprite_->GetSize().y / 2.0f));
			sprite_->SetPosition({ WinApp::kClientWidth / 2.0f,posY });

			//オプションがSHAKEなら揺らす
			if (option_ == Option::SHAKE) {
				//乱数生成器
				static std::random_device rd;
				static std::mt19937 mt(rd());
				//範囲
				std::uniform_real_distribution<float> dist(-10.0f, 10.0f);
				//揺らす
				sprite_->SetShakeOffset({ dist(mt),dist(mt) });
			}
		}
		break;
		case SceneTransitionAnimation::Type::SLIDEDOWN:
		{
			//ポジションを計算
			float posY = MyMath::Lerp(-(sprite_->GetSize().y / 2.0f), WinApp::kClientHeight / 2.0f, static_cast<float>(timer_) / static_cast<float>(time_));
			//クランプ
			posY = std::clamp(posY, -(sprite_->GetSize().y / 2.0f), WinApp::kClientHeight / 2.0f);
			sprite_->SetPosition({ WinApp::kClientWidth / 2.0f,posY });

			//オプションがSHAKEなら揺らす
			if (option_ == Option::SHAKE) {
				//乱数生成器
				static std::random_device rd;
				static std::mt19937 mt(rd());
				//範囲
				std::uniform_real_distribution<float> dist(-10.0f, 10.0f);
				//揺らす
				sprite_->SetShakeOffset({ dist(mt),dist(mt) });
			}
		}
		break;
		default:
			break;
		}

	}

	void SceneTransitionAnimation::EndIn() {
		//状態がEND_INでなければ通らない
		if (state_ != State::END_IN) {
			return;
		}

		//フェードイン終了後の処理
		state_ = State::UPDATE_OUT;
		//フレームをリセット
		timer_ = 0.0f;
		//透明度を1に
		Vector4 color = sprite_->GetColor();
		color.w = 1.0f;
		sprite_->SetColor(color);
		//ポジションを画面中央に
		sprite_->SetPosition({ WinApp::kClientWidth / 2.0f,WinApp::kClientHeight / 2.0f });
		//シェイクオフセットを0に
		sprite_->SetShakeOffset({ 0.0f,0.0f });

	}

	void SceneTransitionAnimation::UpdateOut() {
		//状態がUPDATE_OUTでなければ通らない
		if (state_ != State::UPDATE_OUT) {
			return;
		}
		//時間が達したら終了
		if (timer_ >= time_) {
			//フェードアウト終了
			state_ = State::END_OUT;
		}
		else {
			//時間を進める
			timer_ += kDeltaTime;
		}

		//タイプ毎の処理
		switch (outType_) {
		case SceneTransitionAnimation::Type::FADE:
		{
			//透明度を計算
			float alpha = MyMath::Lerp(1.0f, 0.0f, static_cast<float>(timer_) / static_cast<float>(time_));
			//スプライトの透明度を設定
			Vector4 color = sprite_->GetColor();
			color.w = alpha;
			sprite_->SetColor(color);
		}
		break;
		case SceneTransitionAnimation::Type::SLIDEUP:
		{
			//ポジションを計算
			float posY = MyMath::Lerp(WinApp::kClientHeight / 2.0f, -(sprite_->GetSize().y / 2.0f), static_cast<float>(timer_) / static_cast<float>(time_));
			//クランプ
			posY = std::clamp(posY, -(sprite_->GetSize().y / 2.0f), WinApp::kClientHeight / 2.0f);
			sprite_->SetPosition({ WinApp::kClientWidth / 2.0f,posY });

			//オプションがSHAKEなら揺らす
			if (option_ == Option::SHAKE) {
				//乱数生成器
				static std::random_device rd;
				static std::mt19937 mt(rd());
				//範囲
				std::uniform_real_distribution<float> dist(-10.0f, 10.0f);
				//揺らす
				sprite_->SetShakeOffset({ dist(mt),dist(mt) });
			}
		}
		break;
		case SceneTransitionAnimation::Type::SLIDEDOWN:
		{
			//ポジションを計算
			float posY = MyMath::Lerp(WinApp::kClientHeight / 2.0f, WinApp::kClientHeight + (sprite_->GetSize().y / 2.0f), static_cast<float>(timer_) / static_cast<float>(time_));
			//クランプ
			posY = std::clamp(posY, WinApp::kClientHeight / 2.0f, WinApp::kClientHeight + (sprite_->GetSize().y / 2.0f));
			sprite_->SetPosition({ WinApp::kClientWidth / 2.0f,posY });

			//オプションがSHAKEなら揺らす
			if (option_ == Option::SHAKE) {
				//乱数生成器
				static std::random_device rd;
				static std::mt19937 mt(rd());
				//範囲
				std::uniform_real_distribution<float> dist(-10.0f, 10.0f);
				//揺らす
				sprite_->SetShakeOffset({ dist(mt),dist(mt) });
			}
		}
		break;
		default:
			break;
		}
	}

	void SceneTransitionAnimation::EndOut() {
		//状態がEND_OUTでなければ通らない
		if (state_ != State::END_OUT) {
			return;
		}
		//遷移終了
		state_ = State::END_ALL;
		//シェイクオフセットを0に
		sprite_->SetShakeOffset({ 0.0f,0.0f });
	}

	void SceneTransitionAnimation::EndAll() {
		//遷移終了
		state_ = State::NONE;
		inType_ = Type::NONE;
		outType_ = Type::NONE;
		option_ = Option::NONE;
		time_ = 0.0f;
		timer_ = 0.0f;
		//スプライトを非表示
		sprite_->SetIsDisplay(false);
		//スプライトの透明度を1に
		Vector4 color = sprite_->GetColor();
		color.w = 1.0f;
		sprite_->SetColor(color);
		//遷移中フラグを下ろす
		isTransitioning_ = false;

	}

	void SceneTransitionAnimation::SetTexture(uint32_t _textureHandle) {
		//もし0なら何もしない
		if (_textureHandle == 0u) return;

		sprite_->SetTexture(_textureHandle);

		//テクスチャのサイズを画面サイズ(1280x720)に合わせる
		sprite_->SetSize({ 1280.0f,720.0f });
	}

}