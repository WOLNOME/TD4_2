#include "DevelopCamera.h"
#include "ImGuiManager.h"
#include <algorithm>
#include <numbers>

namespace Norm {

	void DevelopCamera::Initialize() {
		//基盤の初期化
		BaseCamera::Initialize();
		//インプット
		input_ = Input::GetInstance();
	}

	void DevelopCamera::Update() {
		//開発用カメラのマウス操作処理
		enum class Direction {
			Forward,
			Back,
			Right,
			Left
		};

		auto GetDirection = [&](Direction dir) -> Vector3 {
			Matrix4x4 rot = MyMath::CreateRotationFromEulerAngles(
				worldTransform.GetRotate());

			switch (dir) {
			case Direction::Forward: return rot * Vector3(0, 0, 1);
			case Direction::Back:    return rot * Vector3(0, 0, -1);
			case Direction::Right:   return rot * Vector3(1, 0, 0);
			case Direction::Left:    return rot * Vector3(-1, 0, 0);
			}

			return Vector3();
			};
		//新座標を定義
		Vector3 newTranslate = worldTransform.GetTranslate();
		Vector3 newRotate = worldTransform.GetRotate();

		//スクロールで前進後退
		newTranslate += GetDirection(Direction::Forward) * (input_->GetMouseScrollCount() * 1.3f);
		//ドラッグアンドドロップでカメラの向きを変える
		if (input_->PushMouseButton(MouseButton::RightButton)) {
			//マウスの移動幅
			Vector2 moveValue = input_->GetMousePosition();
			//デッドゾーン
			float deadZone = 2.0f;
			if (moveValue.Length() > deadZone) {
				newRotate.x += moveValue.y * 0.001f;
				newRotate.y += moveValue.x * 0.001f;
			}

			//WASDでカメラ移動
			if (input_->PushKey(DIK_W)) {
				newTranslate += GetDirection(Direction::Forward) * 0.1f;
			}
			if (input_->PushKey(DIK_A)) {
				newTranslate += GetDirection(Direction::Left) * 0.1f;
			}
			if (input_->PushKey(DIK_S)) {
				newTranslate += GetDirection(Direction::Back) * 0.1f;
			}
			if (input_->PushKey(DIK_D)) {
				newTranslate += GetDirection(Direction::Right) * 0.1f;
			}

		}
		//カメラの回転制限
		const float maxPitch = (std::numbers::pi_v<float> / 2.0f) - 0.01f;
		newRotate.x = std::clamp(newRotate.x, -maxPitch, maxPitch);

		//新トランスフォームをセット
		worldTransform.SetTranslate(newTranslate);
		worldTransform.SetRotate(newRotate);

		//ベースの更新
		BaseCamera::Update();

	}

	void DevelopCamera::DebugWithImGui() {
#ifdef _DEBUG

		ImGui::Begin("DevelopCamera");
		ImGui::End();

		ImGui::Begin("mouse");
		if (input_->PushMouseButton(MouseButton::LeftButton)) {
			ImGui::Text("LEFT");
		}
		if (input_->PushMouseButton(MouseButton::MiddleButton)) {
			ImGui::Text("MIDDLE");
		}
		if (input_->PushMouseButton(MouseButton::RightButton)) {
			ImGui::Text("RIGHT");
		}
		ImGui::Text("ScrollValue : %f", input_->GetMouseScrollCount());
		ImGui::End();

#endif // _DEBUG
	}

}