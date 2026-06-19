#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <array>
#include <string>
#include "Vector4.h"
#include "Vector3.h"

namespace Norm {

	/// <summary>
	/// 平行光源用データ構造体
	/// </summary>
	struct DirectionalLightData {
		Vector4 color;
		Vector3 direction;
		float intensity = 0.0f;
		uint32_t isActive = 0u;
		float padding[3] = {};
	};

	/// <summary>
	/// 平行光源
	/// </summary>
	class DirectionalLight {
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		DirectionalLight() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~DirectionalLight() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 行列の更新
		/// </summary>
		void Update();
		/// <summary>
		/// デバッグ用ImGui
		/// </summary>
		void DebugWithImGui(const std::wstring& _name);

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// 平行光源のデータを取得
		/// </summary>
		/// <returns>平行光源のデータ</returns>
		const DirectionalLightData& GetData() const { return data_; }

		/// <summary>
		/// 色の取得
		/// </summary>
		/// <returns>色</returns>
		const Vector4 GetColor() { return color_; }
		/// <summary>
		/// 向きの取得
		/// </summary>
		/// <returns>向き</returns>
		const Vector3& GetDirection() const { return direction_; }
		/// <summary>
		/// 輝度の取得
		/// </summary>
		/// <returns>輝度</returns>
		float GetIntensity() const { return intensity_; }
		/// <summary>
		/// 稼働フラグの取得
		/// </summary>
		/// <returns>稼働フラグ</returns>
		bool IsActive() const { return isActive_; }

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// 色を設定する
		/// </summary>
		/// <param name="color">設定する色</param>
		void SetColor(const Vector4& color) { color_ = color; }
		/// <summary>
		/// 向きを設定する
		/// </summary>
		/// <param name="direction">設定する向き</param>
		void SetDirection(const Vector3& direction) { direction_ = direction; }
		/// <summary>
		/// 輝度を設定する
		/// </summary>
		/// <param name="intensity">設定する輝度</param>
		void SetIntensity(float intensity) { intensity_ = intensity; }
		/// <summary>
		/// 稼働フラグを設定する
		/// </summary>
		/// <param name="isActive">稼働フラグ</param>
		void SetActive(bool isActive) { isActive_ = isActive; }

	private:
		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//データ
		DirectionalLightData data_;

		//色
		Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
		//向き
		Vector3 direction_ = { 0.0f, -1.0f, 0.0f };
		//輝度
		float intensity_ = 1.0f;
		//オンオフ
		bool isActive_ = true;

		// コピー禁止
		DirectionalLight(const DirectionalLight&) = delete;
		DirectionalLight& operator=(const DirectionalLight&) = delete;
	};

	static_assert(!std::is_copy_assignable_v<DirectionalLight>);

}