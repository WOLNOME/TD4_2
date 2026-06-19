#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include "Matrix4x4.h"
#include "Vector4.h"
#include "Vector3.h"

namespace Norm {

	/// <summary>
	/// データ構造体(サイズが16の倍数になるようにパディングする！)
	/// </summary>
	struct SpotLightData {
		Vector4 color;					//ライトの色
		Vector3 position;				//ライトの位置
		float intensity = 0.0f;			//輝度
		Vector3 direction;				//スポットライトの方向
		float distance = 0.0f;			//ライトの届く最大距離
		float decay = 0.0f;				//減衰率
		float cosAngle = 0.0f;			//スポットライトの余弦
		float cosFalloffStart = 0.0f;	//フォールオフの開始角度
		uint32_t isActive = 0u;			//稼働させるか
	};

	/// <summary>
	/// スポットライト
	/// </summary>
	class SpotLight {
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		SpotLight() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~SpotLight() = default;
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
		/// スポットライトのデータを取得
		/// </summary>
		/// <returns>スポットライトのデータ</returns>
		const SpotLightData& GetData() { return data_; }

		/// <summary>
		/// 色を取得する
		/// </summary>
		/// <returns>スポットライトの色</returns>
		const Vector4& GetColor() const { return color_; }
		/// <summary>
		/// 位置を取得する
		/// </summary>
		/// <returns>スポットライトの位置</returns>
		const Vector3& GetPosition() const { return position_; }
		/// <summary>
		/// 輝度を取得する
		/// </summary>
		/// <returns>スポットライトの輝度</returns>
		float GetIntensity() const { return intensity_; }
		/// <summary>
		/// ライトの方向を取得する
		/// </summary>
		/// <returns>スポットライトの方向</returns>
		const Vector3& GetDirection() const { return direction_; }
		/// <summary>
		/// 光の有効距離を取得する
		/// </summary>
		/// <returns>光の有効距離</returns>
		float GetDistance() const { return distance_; }
		/// <summary>
		/// 減衰率を取得する
		/// </summary>
		/// <returns>光の減衰率</returns>
		float GetDecay() const { return decay_; }
		/// <summary>
		/// ライトの余弦（内側角）を取得する
		/// </summary>
		/// <returns>ライトの余弦</returns>
		float GetCosAngle() const { return cosAngle_; }
		/// <summary>
		/// フォールオフ開始角度の余弦を取得する
		/// </summary>
		/// <returns>フォールオフ開始角度の余弦</returns>
		float GetCosFalloffStart() const { return cosFalloffStart_; }
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
		/// <param name="intensity">設定する輝度</param>
		void SetIntensity(float intensity) { intensity_ = intensity; }
		/// <summary>
		/// ライトの方向を設定する
		/// </summary>
		/// <param name="direction">設定する方向</param>
		void SetDirection(const Vector3& direction) { direction_ = direction; }
		/// <summary>
		/// 光の有効距離を設定する
		/// </summary>
		/// <param name="distance">設定する有効距離</param>
		void SetDistance(float distance) { distance_ = distance; }
		/// <summary>
		/// 減衰率を設定する
		/// </summary>
		/// <param name="decay">設定する減衰率</param>
		void SetDecay(float decay) { decay_ = decay; }
		/// <summary>
		/// ライトの余弦（内側角）を設定する
		/// </summary>
		/// <param name="cosAngle">設定する余弦値</param>
		void SetCosAngle(float cosAngle) { cosAngle_ = cosAngle; }
		/// <summary>
		/// フォールオフ開始角度の余弦を設定する
		/// </summary>
		/// <param name="cosFalloffStart">設定する余弦値</param>
		void SetCosFalloffStart(float cosFalloffStart) { cosFalloffStart_ = cosFalloffStart; }
		/// <summary>
		/// 稼働フラグを設定する
		/// </summary>
		/// <param name="isActive">稼働フラグ</param>
		void SetActive(bool isActive) { isActive_ = isActive; }

	private:
		/// ============================== ///
		///		メンバ変数(private)
		/// ============================== ///

		//データ
		SpotLightData data_;

		//色
		Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
		//向き
		Vector3 position_ = { 0.0f, 0.0f, 0.0f };
		//輝度
		float intensity_ = 1.0f;
		//ライトの方向
		Vector3 direction_ = { -1.0f,-1.0f,0.0f };
		//光の有効距離
		float distance_ = 5.0f;
		//減衰率
		float decay_ = 0.5f;
		//ライトの余弦
		float cosAngle_ = 0.7f;
		//フォールオフ開始角度
		float cosFalloffStart_ = 1.0f;
		//オンオフ
		bool isActive_ = true;

		// コピー禁止
		SpotLight(const SpotLight&) = delete;
		SpotLight& operator=(const SpotLight&) = delete;
	};

	static_assert(!std::is_copy_assignable_v<SpotLight>);

}