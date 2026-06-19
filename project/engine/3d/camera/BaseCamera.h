#pragma once
#include <d3d12.h>
#include <type_traits>
#include <wrl.h>
#include <cstdint>
#include <list>
#include "MyMath.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "WorldTransform.h"

namespace Norm {

	/// <summary>
	/// VS送信用座標変換行列構造体
	/// </summary>
	struct ViewProjectionTransformationMatrixForVS {
		Matrix4x4 matWorld;
		Matrix4x4 matView;         // ワールド → ビュー変換行列
		Matrix4x4 matProjection;   // ビュー → プロジェクション変換行列
	};
	/// <summary>
	/// PS送信用ワールド座標構造体
	/// </summary>
	struct WorldPositionForPS {
		Vector3 worldPosition;    // カメラ座標（ワールド座標）
	};

	/// <summary>
	/// カメラの基底クラス
	/// </summary>
	class BaseCamera {
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		BaseCamera();
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~BaseCamera() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		virtual void Initialize();
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update();
		/// <summary>
		/// ImGuiデバッグ表示
		/// </summary>
		void DebugWithImGui();

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// ビュー行列を取得する
		/// </summary>
		/// <returns>ビュー行列</returns>
		const Matrix4x4& GetViewMatrix() const { return viewMatrix; }
		/// <summary>
		/// プロジェクション行列を取得する
		/// </summary>
		/// <returns>プロジェクション行列</returns>
		const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix; }
		/// <summary>
		/// ビュープロジェクション行列を取得する
		/// </summary>
		/// <returns>ビュープロジェクション行列</returns>
		const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix; }
		/// <summary>
		/// 縦方向の視野角を取得する
		/// </summary>
		/// <returns>視野角</returns>
		const float GetFovY() { return fovY; }
		/// <summary>
		/// アスペクト比を取得する
		/// </summary>
		/// <returns>アスペクト比</returns>
		const float GetAspectRatio() { return aspectRatio; }
		/// <summary>
		/// ニアクリップ距離を取得する
		/// </summary>
		/// <returns>ニアクリップ距離</returns>
		const float GetNearClip() { return nearClip; }
		/// <summary>
		/// ファークリップ距離を取得する
		/// </summary>
		/// <returns>ファークリップ距離</returns>
		const float GetFarClip() { return farClip; }
		/// <summary>
		/// ビュープロジェクション用の定数バッファを取得する
		/// </summary>
		/// <returns>ビュープロジェクション定数バッファ</returns>
		const Microsoft::WRL::ComPtr<ID3D12Resource>& GetViewProjectionConstBuffer() const { return viewProjectionResource_; }
		/// <summary>
		/// カメラ位置用の定数バッファを取得する
		/// </summary>
		/// <returns>カメラ位置定数バッファ</returns>
		const Microsoft::WRL::ComPtr<ID3D12Resource>& GetCameraPositionConstBuffer() const { return cameraPositionResource_; }

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// 縦方向の視野角を設定する
		/// </summary>
		/// <param name="fovY">視野角</param>
		void SetFovY(float fovY) { this->fovY = fovY; }
		/// <summary>
		/// アスペクト比を設定する
		/// </summary>
		/// <param name="aspectRatio">アスペクト比</param>
		void SetAspectRatio(float aspectRatio) { this->aspectRatio = aspectRatio; }
		/// <summary>
		/// ニアクリップ距離を設定する
		/// </summary>
		/// <param name="nearClip">ニアクリップ距離</param>
		void SetNearClip(float nearClip) { this->nearClip = nearClip; }
		/// <summary>
		/// ファークリップ距離を設定する
		/// </summary>
		/// <param name="farClip">ファークリップ距離</param>
		void SetFarClip(float farClip) { this->farClip = farClip; }

		/// ============================== ///
		///		メンバ変数(public)
		/// ============================== ///

		//ワールドトランスフォーム
		WorldTransform worldTransform;

	protected:
		/// ============================== ///
		///		メンバ変数(protected)
		/// ============================== ///

		// 定数バッファ(座標変換リソース)
		Microsoft::WRL::ComPtr<ID3D12Resource> viewProjectionResource_ = nullptr;
		// マッピング済みアドレス
		ViewProjectionTransformationMatrixForVS* viewProjectionData_ = nullptr;

		// 定数バッファ(カメラ座標リソース)
		Microsoft::WRL::ComPtr<ID3D12Resource> cameraPositionResource_ = nullptr;
		// マッピング済みアドレス
		WorldPositionForPS* cameraPositionData_ = nullptr;

		Matrix4x4 viewMatrix;
		Matrix4x4 projectionMatrix;
		Matrix4x4 viewProjectionMatrix;

		float fovY;               // 水平方向視野角
		float aspectRatio;        // アスペクト比
		float nearClip;           // ニアクリップ距離
		float farClip;            // ファークリップ距離

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// 行列の更新
		/// </summary>
		void UpdateMatrix();

	};

}