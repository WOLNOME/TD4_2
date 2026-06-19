#include "MessageUI.h"
#include <WinApp.h>
#include <TextureManager.h>
#include <SpriteManager.h>
#include <MyMath.h>

using namespace Norm;

void MessageUI::Initialize() {
	//SEの初期化
	messageSE_ = std::make_unique<Audio>();
	messageSE_->Initialize("se/message.mp3");

	//パラメーターの読み込み
	param_ = JsonUtil::GetJsonData("Resources/parameters/messageUI");

	//スプライトの生成・初期化
	for (int i = 0; i < kMaxSpriteNum_; ++i) {
		sprites_[i] = std::make_unique<Sprite>();
		sprites_[i]->Initialize(TextTag{}, SpriteManager::GetInstance()->GenerateName("messageUI"), Order::Front2);
		sprites_[i]->SetIsDisplay(false);
		sprites_[i]->SetAnchorPoint({ 0.5f, 0.5f });
	}

	//ヘッダーの生成・初期化
	{
		Vector2 centerPos = { param_["header"]["centerPos"]["x"], param_["header"]["centerPos"]["y"] };
		thHeader_ = TextureManager::GetInstance()->LoadTexture("messageUI.png");
		spriteHeader_ = std::make_unique<Sprite>();
		spriteHeader_->Initialize(SpriteTag{}, SpriteManager::GetInstance()->GenerateName("messageUIHeader"), Order::Front2, thHeader_);
		spriteHeader_->SetAnchorPoint({ 0.5f, 0.5f });
		spriteHeader_->SetPosition(centerPos);
	}


	//基本のテキストパラメーターを設定
	auto textColor = param_["text"]["color"];
	baseTextParam_.text = L"";
	baseTextParam_.font = (Font)param_["text"]["font"];
	baseTextParam_.fontStyle = (FontStyle)param_["text"]["fontStyle"];
	baseTextParam_.size = param_["text"]["size"];
	baseTextParam_.color = Vector4(
		textColor[0].get<float>(),
		textColor[1].get<float>(),
		textColor[2].get<float>(),
		textColor[3].get<float>()
	);
}

void MessageUI::Update() {
	// メッセージが無い場合は何もしない
	if (messages_.empty()) return;

	// メッセージの削除
	DeleteMessage();

	// メッセージの更新
	UpdateMessage();

	//スプライトの更新
	UpdateSprite();
}

void MessageUI::DebugWithImGui() {
#ifdef _DEBUG
#endif // _DEBUG
}

uint32_t MessageUI::AddMessage(const std::wstring& _text, float _displayTime, bool _isBlinking) {
	//新たなメッセージを定義し、パラメーターを設定する
	MessageData newMessage;
	newMessage.id = messageIdCounter_;
	newMessage.text = _text;
	newMessage.currentText = L"";
	newMessage.inputTimer = param_["inputTime"];
	if (_displayTime != 0.0f) {
		newMessage.displayTimer = _displayTime;
	}
	else {
		newMessage.displayTimer = param_["displayTime"];
	}
	newMessage.disappearTimer = param_["disappearTime"];
	newMessage.blinkTimer = param_["blinkInterval"];
	newMessage.state = MessageState::kInputting;
	newMessage.isFinished = false;
	newMessage.isBlinking = _isBlinking;
	newMessage.textHandle = TextTextureManager::GetInstance()->LoadTextTexture(baseTextParam_);
	messages_.push_back(newMessage);
	//SEを再生
	messageSE_->Play(false, 1.0f);

	//メッセージのハンドルを返す
	return messageIdCounter_++;
}

void MessageUI::FinishMessage(uint32_t _messageId) {
	// 指定されたIDのメッセージを消滅中に移行させる
	for (auto& message : messages_) {
		if (message.id == _messageId) {
			//もし入力中ならSEを消す
			if (message.state == MessageState::kInputting) {
				messageSE_->Stop();
			}

			message.state = MessageState::kDisappearing;

			break;
		}
	}
}

void MessageUI::DeleteMessage() {
	// 終了判定の出たメッセージをリストから削除
	for (auto it = messages_.begin(); it != messages_.end();) {
		if (it->isFinished) {
			it = messages_.erase(it); // メッセージを削除
		}
		else {
			++it; // 次のメッセージへ
		}
	}
}

void MessageUI::UpdateMessage() {
	for (auto& message : messages_) {
		//点滅処理
		if (message.isBlinking) {
			message.blinkTimer -= kDeltaTime;
			float blinkInterval = param_["blinkInterval"];
			if (message.blinkTimer <= 0.0f) {
				message.blinkTimer = blinkInterval;
				//テキストの表示・非表示を切り替え
				Vector4 currentColor = TextTextureManager::GetInstance()->GetTextColor(message.textHandle);
				if (currentColor.w == 1.0f) {
					//非表示にする
					TextTextureManager::GetInstance()->EditTextColor(message.textHandle, { currentColor.x, currentColor.y, currentColor.z, 0.0f });
				}
				else {
					//表示する
					TextTextureManager::GetInstance()->EditTextColor(message.textHandle, { currentColor.x, currentColor.y, currentColor.z, 1.0f });
				}
			}
		}

		switch (message.state) {
		case MessageState::kInputting:
		{
			// 入力中
			message.inputTimer -= kDeltaTime;
			if (message.inputTimer <= 0.0f) {
				//表示するテキストを更新
				TextTextureManager::GetInstance()->EditTextString(message.textHandle, message.text);

				message.inputTimer = 0.0f;
				message.state = MessageState::kDisplaying; // 表示中へ移行

				//入力中SEを停止
				messageSE_->Stop();

				continue;	//次のメッセージへ
			}
			// 入力中のテキストを更新
			float inputTime = param_["inputTime"];
			message.currentText = message.text.substr(0, static_cast<size_t>(message.text.size() * (1.0f - message.inputTimer / inputTime)));
			TextTextureManager::GetInstance()->EditTextString(message.textHandle, message.currentText);

			break;
		}
		case MessageState::kDisplaying:
		{
			// 表示中
			message.displayTimer -= kDeltaTime;
			if (message.displayTimer <= 0.0f) {
				message.displayTimer = 0.0f;
				message.state = MessageState::kDisappearing; // 消滅中へ移行

				continue;	// 次のメッセージへ
			}
			break;
		}
		case MessageState::kDisappearing:
		{
			// 消滅中
			message.disappearTimer -= kDeltaTime;
			if (message.disappearTimer <= 0.0f) {
				message.disappearTimer = 0.0f;
				message.state = MessageState::kFinished; // 完了へ移行
				continue;	// 次のメッセージへ
			}
			// 透明にしていく
			float disappearTime = param_["disappearTime"];
			float alpha = message.disappearTimer / disappearTime;
			TextTextureManager::GetInstance()->EditTextColor(message.textHandle, { baseTextParam_.color.x, baseTextParam_.color.y, baseTextParam_.color.z, alpha });

			break;
		}
		case MessageState::kFinished:
			// 完了
			message.isFinished = true;
			break;
		default:
			break;
		}
	}
}

void MessageUI::UpdateSprite() {
	int index = 0;

	float interval = param_["text"]["interval"];
	//登録されたメッセージのうち先入れされたメッセージを表示する処理
	for (auto it = messages_.rbegin(); it != messages_.rend(); ++it) {
		if (index >= kMaxSpriteNum_) break;

		Vector2 centerPos = { param_["text"]["centerPos"]["x"], param_["text"]["centerPos"]["y"] };
		centerPos.y += index * interval; // スプライト同士の間隔を空ける

		sprites_[index]->SetIsDisplay(true);
		sprites_[index]->SetPosition(centerPos);
		sprites_[index]->SetTexture(it->textHandle);

		++index;
	}

	// 残りのスプライトを非表示にする
	for (; index < kMaxSpriteNum_; ++index) {
		sprites_[index]->SetIsDisplay(false);
	}
}
