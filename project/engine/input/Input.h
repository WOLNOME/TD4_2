#pragma once
#include <wrl.h>
#include <memory>
#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#include <Xinput.h>
#include "Vector2.h"

namespace Norm {

	/// <summary>
	/// マウスのコマンド
	/// </summary>
	enum class MouseButton {
		LeftButton,    // 左ボタン
		RightButton,   // 右ボタン
		MiddleButton   // 中央ボタン
	};

	/// <summary>
	/// XInputの各種ボタンマスク
	/// </summary>
	static const WORD kXInputButtonMasks[] = {
		XINPUT_GAMEPAD_A,				//Aボタン
		XINPUT_GAMEPAD_B,				//Bボタン
		XINPUT_GAMEPAD_X,				//Xボタン
		XINPUT_GAMEPAD_Y,				//Yボタン
		XINPUT_GAMEPAD_DPAD_UP,			//十字上ボタン
		XINPUT_GAMEPAD_DPAD_DOWN,		//十字下ボタン
		XINPUT_GAMEPAD_DPAD_LEFT,		//十字左ボタン
		XINPUT_GAMEPAD_DPAD_RIGHT,		//十字右ボタン
		XINPUT_GAMEPAD_LEFT_SHOULDER,	//LBボタン
		XINPUT_GAMEPAD_RIGHT_SHOULDER,	//RBボタン
		XINPUT_GAMEPAD_LEFT_THUMB,		//左スティック押し込み
		XINPUT_GAMEPAD_RIGHT_THUMB,		//右スティック押し込み
		XINPUT_GAMEPAD_START,			//スタートボタン（+）
		XINPUT_GAMEPAD_BACK				//バックボタン（-）
	};

	/// <summary>
	/// ゲームパッドのボタン
	/// </summary>
	enum class GamePadButton {
		A,				  // Aボタン
		B,                // Bボタン
		X,                // Xボタン
		Y,                // Yボタン
		DPAD_UP,          // 十字キー 上
		DPAD_DOWN,        // 十字キー 下
		DPAD_LEFT,        // 十字キー 左
		DPAD_RIGHT,       // 十字キー 右
		LB,               // 左ショルダー
		RB,               // 右ショルダー
		L_STICK_PUSH,     // 左スティック押し込み
		R_STICK_PUSH,     // 右スティック押し込み
		START,            // Start
		BACK,             // Back

		COUNT             // 総数（配列サイズ用）
	};

	/// <summary>
	/// デバイスから入力される操作信号を判定するクラス
	/// シングルトンパターンで実装
	/// </summary>
	class Input {
	private://コンストラクタ等の隠蔽
		static std::unique_ptr<Input> instance_;

		Input() = default;//コンストラクタ隠蔽
		~Input() = default;//デストラクタ隠蔽
		Input(Input&) = delete;//コピーコンストラクタ封印
		Input& operator=(Input&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<Input>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		/// <returns></returns>
		static Input* GetInstance();

		//namespace省略
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;//エイリアステンプレート

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// ============================== ///
		///		コマンド操作関数
		/// ============================== ///

		/// <summary>
		/// 指定したマウスボタンが現在押されているかを判定する
		/// </summary>
		/// <param name="button">判定対象のマウスボタン</param>
		/// <returns>押されていれば true、押されていなければ false</returns>
		bool PushMouseButton(MouseButton button);
		/// <summary>
		/// 指定したマウスボタンが「このフレームで押された瞬間」であるかを判定する
		/// </summary>
		/// <param name="button">判定対象のマウスボタン</param>
		/// <returns>押された瞬間であれば true、そうでなければ false</returns>
		bool TriggerMouseButton(MouseButton button);
		/// <summary>
		/// 指定したキーが現在押されているかを判定する
		/// </summary>
		/// <param name="keyNumber">判定対象のキー番号</param>
		/// <returns>押されていれば true、押されていなければ false</returns>
		bool PushKey(BYTE keyNumber);
		/// <summary>
		/// 指定したキーが「このフレームで押された瞬間」であるかを判定する
		/// </summary>
		/// <param name="keyNumber">判定対象のキー番号</param>
		/// <returns>押された瞬間であれば true、そうでなければ false</returns>
		bool TriggerKey(BYTE keyNumber);
		/// <summary>
		/// 指定したキーが「このフレームで離された瞬間」であるかを判定する
		/// </summary>
		/// <param name="keyNumber">判定対象のキー番号</param>
		/// <returns></returns>
		bool ReleaseKey(BYTE keyNumber);
		/// <summary>
		/// 指定したゲームパッドボタンが現在押されているかを判定する
		/// </summary>
		/// <param name="button">判定対象のゲームパッドボタン</param>
		/// <returns>押されていれば true、押されていなければ false</returns>
		bool PushPadButton(GamePadButton button);
		/// <summary>
		/// 指定したゲームパッドボタンが「このフレームで押された瞬間」であるかを判定する
		/// </summary>
		/// <param name="button">判定対象のゲームパッドボタン</param>
		/// <returns>押された瞬間であれば true、そうでなければ false</returns>
		bool TriggerPadButton(GamePadButton button);
		/// <summary>
		/// 指定したゲームパッドボタンが「このフレームで離された瞬間」であるかを判定する
		/// </summary>
		/// <param name="button">判定対象のゲームパッドボタン</param>
		/// <returns>離された瞬間であれば true、そうでなければ false</returns>
		bool ReleasePadButton(GamePadButton button);
		/// <summary>
		/// 現在のマウスの座標（スクリーン座標）
		/// </summary>
		/// <returns>マウスのポジション</returns>
		Vector2 GetMousePosition();
		/// <summary>
		/// 現在のマウスの移動量
		/// </summary>
		/// <returns>マウス移動量（スクリーン座標系）</returns>
		Vector2 GetMouseMoveValue();
		/// <summary>
		/// マウスホイールの回転量を取得する
		/// </summary>
		/// <returns>
		/// スクロール量（上方向が正の値、下方向が負の値）
		/// </returns>
		float GetMouseScrollCount();
		/// <summary>
		/// ゲームパッドの左スティックの入力方向を取得する
		/// </summary>
		/// <returns>左スティックの入力方向（正規化済みベクトル）</returns>
		Vector2 GetLStickDir();
		/// <summary>
		/// ゲームパッドの右スティックの入力方向を取得する
		/// </summary>
		/// <returns>右スティックの入力方向（正規化済みベクトル）</returns>
		Vector2 GetRStickDir();
		/// <summary>
		/// LTの取得
		/// </summary>
		/// <returns>LTの値(0.0~1.0)</returns>
		float GetLT();
		/// <summary>
		/// RTの取得
		/// </summary>
		/// <returns>RTの値(0.0~1.0)</returns>
		float GetRT();
		/// <summary>
		/// マウスカーソルの表示／非表示を設定する
		/// </summary>
		/// <param name="_isDisplay">true ならカーソルを表示、false なら非表示</param>
		void SetIsMouseDisplay(bool _isDisplay);
		/// <summary>
		/// マウスカーソルの固定状態を設定する（画面中央に固定するなど）
		/// </summary>
		/// <param name="_isMiddle">true ならカーソルを固定、false なら自由に動かす</param>
		void SetIsMouseFixed(bool _isMiddle);

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// DirectInputの初期化を行う
		/// </summary>
		void InitDirectInput();
		/// <summary>
		/// マウスデバイスを生成する
		/// </summary>
		void GenerateMouse();
		/// <summary>
		/// キーボードデバイスを生成する
		/// </summary>
		void GenerateKeyboard();

		/// <summary>
		/// ゲームパッドの状態を取得
		/// </summary>
		/// <returns></returns>
		XINPUT_STATE GetGamePadState();

		/// <summary>
		/// ゲームパッドデバイスを生成する
		/// </summary>
		void GenerateGamepad();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//DiretInput
		ComPtr<IDirectInput8> directInput_;

		//マウスデバイス
		ComPtr<IDirectInputDevice8> mouseDevice_;
		DIMOUSESTATE mouseState_;	//マウスの状態
		DIMOUSESTATE preMouseState_;	//前フレームのマウスの状態

		//キーボードデバイス
		ComPtr<IDirectInputDevice8> keyboardDevice_;
		BYTE keys_[256] = {};	//全キーの状態
		BYTE preKeys_[256] = {};	//前フレームの全キーの状態

		//ゲームパッドデバイス
		XINPUT_STATE gamePadState_;	//ゲームパッドの状態
		static const int kGamePadButtonNum_ = 14;	//ゲームパッドのボタンの数
		bool buttonStates_[kGamePadButtonNum_];	//ゲームパッドのボタンの状態
		bool preButtonStates_[kGamePadButtonNum_];	//前フレームのゲームパッドのボタンの状態
	};

}