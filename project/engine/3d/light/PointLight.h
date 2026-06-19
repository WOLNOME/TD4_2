#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include "Vector4.h"
#include "Vector3.h"

namespace Norm {

	/// <summary>
	/// 点光源データ構造体
	/// </summary>
	struct PointLightData {
		Vector4 color;		//ライトの色
		Vector3 position;	//ライトの位置
		float intensity = 0.0f;	//輝度
		float radius = 0.0f;		//ライトの届く最大距離
		float decay = 0.0f;		//減衰率
		uint32_t isActive = 0u;
		float padding[1] = {};
	};

	/// <summary>
	/// 点光源
	/// </summary>
	class PointLight {
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		PointLight() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~PointLight() = default;
		/// <summary>
		/// 行列の更新
		/// </summary>
		void Update();
		/// <summary>
		/// デバッグ用ImGui
		/// </summary>
		/// <param name="_name">名前</param>
		void DebugWithImGui(const std::wstring& _name);

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// 平行光源のデータを取得
		/// </summary>
		/// <returns>スポットライトのデータ</returns>
		const PointLightData& GetData() { return data_; }

		/// <summary>
		/// 色を取得する
		/// </summary>
		/// <returns>ポイントライトの色</returns>
		const Vector4& GetColor() const { return color_; }
		/// <summary>
		/// 位置を取得する
		/// </summary>
		/// <returns>ポイントライトの位置</returns>
		const Vector3& GetPosition() const { return position_; }
		/// <summary>
		/// 輝度を取得する
		/// </summary>
		/// <returns>ポイントライトの輝度</returns>
		float GetIntensity() const { return intensity_; }
		/// <summary>
		/// 有効距離を取得する
		/// </summary>
		/// <returns>光の有効距離</returns>
		float GetRadius() const { return radius_; }
		/// <summary>
		/// 減衰率を取得する
		/// </summary>
		/// <returns>光の減衰率</returns>
		float GetDecay() const { return decay_; }
		/// <summary>
		/// 稼働フラグを取得する
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
		/// 位置を設定する
		/// </summary>
		/// <param name="position">設定する位置</param>
		void SetPosition(const Vector3& position) { position_ = position; }
		/// <summary>
		/// 輝度を設定する
		/// </summary>
		/// <param name="intencity">設定する輝度</param>
		void SetIntensity(float intensity) { intensity_ = intensity; }
		/// <summary>
		/// 有効距離を設定する
		/// </summary>
		/// <param name="radius">設定する光の有効距離</param>
		void SetRadius(float radius) { radius_ = radius; }
		/// <summary>
		/// 減衰率を設定する
		/// </summary>
		/// <param name="decay">設定する減衰率</param>
		void SetDecay(float decay) { decay_ = decay; }
		/// <summary>
		/// 稼働フラグを設定する
		/// </summary>
		/// <param name="isActive">稼働フラグ</param>
		void SetActive(bool isActive) { isActive_ = isActive; }

	private:
		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		// データ
		PointLightData data_;

		//色
		Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
		//向き
		Vector3 position_ = { 0.0f, 0.0f, 0.0f };
		//輝度
		float intensity_ = 1.0f;
		//光の有効距離
		float radius_ = 5.0f;
		//減衰率
		float decay_ = 0.5f;
		//オンオフ
		bool isActive_ = true;

		// コピー禁止
		PointLight(const PointLight&) = delete;
		PointLight& operator=(const PointLight&) = delete;
	};

	static_assert(!std::is_copy_assignable_v<PointLight>);

}