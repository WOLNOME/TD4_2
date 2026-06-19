#pragma once
#include "BaseCamera.h"
#include "Input.h"
#include "Vector2.h"

namespace Norm {

	/// <summary>
	/// 開発用カメラの管理を行うクラス
	/// </summary>
	class DevelopCamera : public BaseCamera {
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		DevelopCamera() : BaseCamera() {}
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~DevelopCamera() = default;
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize() override;
		/// <summary>
		/// 更新
		/// </summary>
		void Update() override;
		/// <summary>
		/// ImGui操作
		/// </summary>
		void DebugWithImGui();

	private:
		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		Input* input_ = nullptr;

		Vector2 start = { 0.0f,0.0f };

	};

}