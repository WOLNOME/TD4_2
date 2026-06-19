#pragma once
#include <TextTextureManager.h>
#include <Audio.h>
#include <Sprite.h>
#include <string>
#include <list>
#include <array>
#include <Handle.h>
#include <JsonUtil.h>

/// <summary>
/// メッセージ(通知)UIを管理するクラス
/// </summary>
class MessageUI {
private:
	/// ============================== ///
	///		列挙体
	/// ============================== ///

	/// <summary>
	/// メッセージの状態
	/// </summary>
	enum class MessageState {
		kInputting,		// 入力中
		kDisplaying,		// 表示中
		kDisappearing,	// 消滅中
		kFinished,		// 完了
	};

	/// ============================== ///
	///		構造体
	/// ============================== ///

	/// <summary>
	/// 1つのメッセージに必要なデータ
	/// </summary>
	struct MessageData {
		uint32_t id;				// メッセージのID
		std::wstring text;			// 全文テキスト
		std::wstring currentText;	// 現在の入力中のテキスト
		Norm::Handle textHandle;			// テキストのハンドル
		float inputTimer;			// 入力タイマー
		float displayTimer;			// 表示タイマー
		float disappearTimer;		// 消滅タイマー
		float blinkTimer;			// 点滅タイマー
		MessageState state;			// メッセージの状態
		bool isFinished;			// 完了したかどうか
		bool isBlinking;			// 点滅するかどうか
	};

public:
	/// ============================== ///
	///		メンバ関数
	/// ============================== ///

	/// <summary>
	/// コンストラクタ
	/// </summary>
	MessageUI() = default;
	/// <summary>
	/// デストラクタ
	/// </summary>
	~MessageUI() = default;
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// デバッグ用ImGui
	/// </summary>
	void DebugWithImGui();

	/// <summary>
	/// メッセージの追加
	/// </summary>
	/// <param name="_text">テキスト</param>
	/// <param name="_displayTime">表示時間</param>
	/// <param name="_isBlinking"点滅させるか></param>
	/// <returns>メッセージのハンドル</returns>
	uint32_t AddMessage(const std::wstring& _text, float _displayTime = 0.0f, bool _isBlinking = false);

	/// <summary>
	/// メッセージの完了
	/// </summary>
	/// <param name="_messageId">メッセージのハンドル</param>
	void FinishMessage(uint32_t _messageId);

private:
	/// ============================== ///
	///		非公開メンバ関数
	/// ============================== ///

	/// <summary>
	/// メッセージの削除
	/// </summary>
	void DeleteMessage();
	/// <summary>
	/// メッセージの更新処理
	/// </summary>
	void UpdateMessage();
	/// <summary>
	/// スプライトの更新
	/// </summary>
	void UpdateSprite();

	/// ============================== ///
	///		メンバ変数
	/// ============================== ///
	
	//SE
	std::unique_ptr<Norm::Audio> messageSE_ = nullptr;	//カタカタ

	//パラメーター
	json param_;

	//メッセージのコンテナ
	std::list<MessageData> messages_;
	//メッセージIDカウンター
	uint32_t messageIdCounter_ = 0u;

	//スプライト
	static const int kMaxSpriteNum_ = 5;	//最大スプライト数
	std::array<std::unique_ptr<Norm::Sprite>, kMaxSpriteNum_> sprites_;

	//基本のテキストパラメーター
	Norm::TextParam baseTextParam_;

	//ヘッダー
	uint32_t thHeader_;
	std::unique_ptr<Norm::Sprite> spriteHeader_;

};

