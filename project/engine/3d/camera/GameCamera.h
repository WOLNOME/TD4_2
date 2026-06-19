#pragma once
#include "BaseCamera.h"

namespace Norm {

	/// <summary>
	/// ゲームカメラを管理するクラス
	/// </summary>
	class GameCamera : public BaseCamera {
	private:
		/// ============================== ///
		///		構造体
		/// ============================== ///

		// カメラの揺れを管理する構造体
		struct ShakeData {
			float maxTime;		// 揺れの最大時間
			float time;			// 揺れの時間
			float maxPower;		// 揺れの最大強さ
			float power;		// 揺れの強さ
		};

	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		GameCamera() : BaseCamera() {}
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~GameCamera() = default;
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

		/// <summary>
		/// カメラの揺れを登録する
		/// </summary>
		/// <param name="time">時間</param>
		/// <param name="power">強さ</param>
		void RegistShake(float time, float power = 0.5f);

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// カメラが揺れているか取得する
		/// </summary>
		/// <returns>カメラが揺れているか</returns>
		bool GetIsShake();
		/// <summary>
		/// カメラの揺れの強さを取得する
		/// </summary>
		/// <returns>カメラの揺れの強さ</returns>
		float GetShakePower() const;

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// カメラの揺れを更新する
		/// </summary>
		void UpdateShake();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		// カメラの揺れを管理する変数
		std::list<ShakeData> shakeList_;
		Vector3 shakeOffset_ = { 0.0f,0.0f,0.0f };
	};

}
