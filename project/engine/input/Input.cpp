#include "Input.h"
#include <cassert>
#include <Windows.h>
#include "WinApp.h"
#include "Logger.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"XInput.lib")
#pragma comment(lib,"dxguid.lib")

namespace Norm {

	std::unique_ptr<Input> Input::instance_ = nullptr;

	Input* Input::GetInstance() {
		if (!instance_) {
			instance_ = std::unique_ptr<Input>(new Input());
		}
		return instance_.get();
	}

	void Input::Initialize() {
		//ダイレクトインプットの初期化
		InitDirectInput();
		//マウスの生成
		GenerateMouse();
		//キーボードの生成
		GenerateKeyboard();
		//ゲームパッドの生成
		GenerateGamepad();
	}

	void Input::Update() {
		//マウスの前フレーム状態更新
		preMouseState_ = mouseState_;
		//キーボードの前フレーム状態更新
		memcpy(preKeys_, keys_, sizeof(keys_));
		//ゲームパッドの前フレーム状態更新
		memcpy(preButtonStates_, buttonStates_, sizeof(buttonStates_));

		//マウスの状態更新
		mouseDevice_->Acquire();
		mouseDevice_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState_);

		//キーボードの状態更新
		keyboardDevice_->Acquire();
		keyboardDevice_->GetDeviceState(sizeof(keys_), keys_);

		//ゲームパッドの状態更新
		gamePadState_ = GetGamePadState();
		for (int i = 0; i < kGamePadButtonNum_; i++) {
			buttonStates_[i] = (gamePadState_.Gamepad.wButtons & kXInputButtonMasks[i]) == kXInputButtonMasks[i];
		}
	}

	void Input::Finalize() {
		//インスタンスを削除
		instance_.reset();
	}

	void Input::InitDirectInput() {
		HRESULT hr;
		//DirectInputの初期化
		hr = DirectInput8Create(
			WinApp::GetInstance()->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
			(void**)&directInput_, nullptr);
		assert(SUCCEEDED(hr));
	}

	void Input::GenerateKeyboard() {
		HRESULT hr;

		//キーボードデバイスの生成
		keyboardDevice_ = nullptr;
		hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboardDevice_, NULL);
		assert(SUCCEEDED(hr));
		//入力データ形式のセット
		hr = keyboardDevice_->SetDataFormat(&c_dfDIKeyboard);
		assert(SUCCEEDED(hr));
		//ウィンドウがないためスキップ
		if (!IsWindow(WinApp::GetInstance()->GetHwnd())) {
			Logger::Log("ウィンドウ消えてるからSetCooperativeLevelスキップ");
			return;
		}
		//排他制御レベルのセット
		hr = keyboardDevice_->SetCooperativeLevel(
			WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		assert(SUCCEEDED(hr));
	}

	void Input::GenerateMouse() {
		HRESULT hr;

		// マウスデバイスの生成
		mouseDevice_ = nullptr;
		hr = directInput_->CreateDevice(GUID_SysMouse, &mouseDevice_, NULL);
		if (FAILED(hr)) {
			// マウスが接続されていない場合、処理をスキップ
			mouseDevice_ = nullptr;
		}
		else {
			// マウスが見つかった場合のみ初期化処理を行う
			hr = mouseDevice_->SetDataFormat(&c_dfDIMouse);
			assert(SUCCEEDED(hr));

			// 協調モードの設定
			hr = mouseDevice_->SetCooperativeLevel(WinApp::GetInstance()->GetHwnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
			assert(SUCCEEDED(hr));
		}
	}

	XINPUT_STATE Input::GetGamePadState() {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		// ゲームパッドの状態を取得
		XInputGetState(0, &state);
		return state;
	}

	void Input::GenerateGamepad() {
		//状態の初期化
		for (int i = 0; i < kGamePadButtonNum_; i++) {
			preButtonStates_[i] = false;
			buttonStates_[i] = false;
		}
	}

	bool Input::PushMouseButton(MouseButton button) {
		// マウスボタンが押されているか確認
		switch (button) {
		case MouseButton::LeftButton:
			return (mouseState_.rgbButtons[0] & 0x80) != 0;
		case MouseButton::RightButton:
			return (mouseState_.rgbButtons[1] & 0x80) != 0;
		case MouseButton::MiddleButton:
			return (mouseState_.rgbButtons[2] & 0x80) != 0;
		default:
			return false;
		}
	}

	bool Input::TriggerMouseButton(MouseButton button) {
		// マウスボタンが押された瞬間か確認
		switch (button) {
		case MouseButton::LeftButton:
			return (mouseState_.rgbButtons[0] & 0x80) != 0 && !(preMouseState_.rgbButtons[0] & 0x80);
		case MouseButton::RightButton:
			return (mouseState_.rgbButtons[1] & 0x80) != 0 && !(preMouseState_.rgbButtons[1] & 0x80);
		case MouseButton::MiddleButton:
			return (mouseState_.rgbButtons[2] & 0x80) != 0 && !(preMouseState_.rgbButtons[2] & 0x80);
		default:
			return false;
		}
	}

	bool Input::PushKey(BYTE keyNumber) {
		//状態を返す
		return keys_[keyNumber];
	}

	bool Input::TriggerKey(BYTE keyNumber) {
		//状態を返す
		return (keys_[keyNumber] && !preKeys_[keyNumber]);
	}

	bool Input::ReleaseKey(BYTE keyNumber) {
		//状態を返す
		return (!keys_[keyNumber] && preKeys_[keyNumber]);
	}

	bool Input::PushPadButton(GamePadButton button) {
		//状態を返す
		return buttonStates_[(int)button];
	}

	bool Input::TriggerPadButton(GamePadButton button) {
		//状態を返す
		return (!preButtonStates_[(int)button] && buttonStates_[(int)button]);
	}

	bool Input::ReleasePadButton(GamePadButton button) {
		//状態を返す
		return (preButtonStates_[(int)button] && !buttonStates_[(int)button]);
	}

	Vector2 Input::GetMousePosition() {
		return Vector2(static_cast<float>(mouseState_.lX), static_cast<float>(mouseState_.lY));
	}

	float Input::GetMouseScrollCount() {
		// 1回のスクロールは120単位
		const float scrollPerUnit = 120.0f;

		// lZはマウスホイールの変化量（単位は120ごと）
		return static_cast<float>(mouseState_.lZ) / scrollPerUnit;
	}

	Vector2 Input::GetLStickDir() {
		const short deadZone = 200; // デッドゾーンの設定
		short x = gamePadState_.Gamepad.sThumbLX;
		short y = gamePadState_.Gamepad.sThumbLY;

		// デッドゾーン内の値は 0 にする
		if (abs(x) < deadZone) x = 0;
		if (abs(y) < deadZone) y = 0;

		//状態を返す
		return Vector2(static_cast<float>(x) / 32768.0f, static_cast<float>(y) / 32768.0f);
	}

	Vector2 Input::GetRStickDir() {
		const short deadZone = 200; // デッドゾーンの設定
		short x = gamePadState_.Gamepad.sThumbRX;
		short y = gamePadState_.Gamepad.sThumbRY;

		// デッドゾーン内の値は 0 にする
		if (abs(x) < deadZone) x = 0;
		if (abs(y) < deadZone) y = 0;

		//状態を返す
		return Vector2(static_cast<float>(x) / 32768.0f, static_cast<float>(y) / 32768.0f);
	}

	float Input::GetLT() {
		BYTE trigger = gamePadState_.Gamepad.bLeftTrigger;
		if (trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
			return static_cast<float>(trigger) / 255.0f;
		}
		return 0.0f;
	}

	float Input::GetRT() {
		BYTE trigger = gamePadState_.Gamepad.bRightTrigger;
		if (trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
			return static_cast<float>(trigger) / 255.0f;
		}
		return 0.0f;
	}

	void Input::SetIsMouseDisplay(bool _isDisplay) {
		if (_isDisplay) {
			while (ShowCursor(TRUE) < 0);
		}
		else {
			while (ShowCursor(FALSE) >= 0);
		}
	}

	void Input::SetIsMouseFixed(bool _isMiddle) {
		if (_isMiddle) {
			RECT rect;
			GetClientRect(WinApp::GetInstance()->GetHwnd(), &rect);
			POINT ul = { rect.left, rect.top };
			POINT lr = { rect.right, rect.bottom };

			// クライアント座標をスクリーン座標に変換
			ClientToScreen(WinApp::GetInstance()->GetHwnd(), &ul);
			ClientToScreen(WinApp::GetInstance()->GetHwnd(), &lr);

			RECT clipRect = { ul.x, ul.y, lr.x, lr.y };
			ClipCursor(&clipRect);
		}
		else {
			ClipCursor(NULL);
		}
	}

}