#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Matrix4x4.h"
#include "Vector3.h"
#include "MyMath.h"

namespace Norm {

	/// <summary>
	/// 頂点シェーダー用ワールド変換行列データ
	/// </summary>
	struct WorldTransformationMatrixForVS {
		Matrix4x4 matWorld; // ローカル → ワールド変換行列
		Matrix4x4 matWorldInverseTranspose; //ワールド逆転置行列
	};

	/// <summary>
	/// ワールド変換データ
	/// </summary>
	class WorldTransform {
	public:
		//ムーブ許可
		WorldTransform(WorldTransform&&) noexcept = default;
		WorldTransform& operator=(WorldTransform&&) noexcept = default;

		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		WorldTransform() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~WorldTransform() = default;
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// データの更新
		/// </summary>
		void UpdateMatrix();

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// 定数バッファの取得
		/// </summary>
		/// <returns>定数バッファ</returns>
		const Microsoft::WRL::ComPtr<ID3D12Resource>& GetConstBuffer() const { return resource_; }

		/// <summary>
		/// ローカルスケールを取得する
		/// </summary>
		/// <returns>ローカルスケール</returns>
		const Vector3& GetScale() const { return scale_; }
		/// <summary>
		/// ローカル回転角を取得する
		/// </summary>
		/// <returns>ローカル回転角</returns>
		const Vector3& GetRotate() const { return rotate_; }
		/// <summary>
		/// ローカル座標を取得する
		/// </summary>
		/// <returns>ローカル座標</returns>
		const Vector3& GetTranslate() const { return translate_; }
		/// <summary>
		/// ワールド座標を取得する
		/// </summary>
		/// <returns>ワールド座標</returns>
		const Vector3& GetWorldTranslate() const { return worldTranslate_; }
		/// <summary>
		/// 前フレームのワールド座標を取得する
		/// </summary>
		/// <returns>前フレームのワールド座標</returns>
		const Vector3& GetPreWorldTranslate() const { return preWorldTranslate_; }

		/// <summary>
		/// ワールド変換行列を取得する
		/// </summary>
		/// <returns>ワールド変換行列</returns>
		const Matrix4x4& GetWorldMatrix() const { return matWorld_; }

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// ローカルスケールを設定する
		/// </summary>
		/// <param name="scale">設定するスケール</param>
		void SetScale(const Vector3& scale) { scale_ = scale; }
		/// <summary>
		/// ローカル回転角を設定する
		/// </summary>
		/// <param name="rotate">設定する回転角</param>
		void SetRotate(const Vector3& rotate) { rotate_ = rotate; }
		/// <summary>
		/// ローカル座標を設定する
		/// </summary>
		/// <param name="translate">設定する座標</param>
		void SetTranslate(const Vector3& translate) { translate_ = translate; }
	
		/// <summary>
		/// 親となるワールド変換を設定する
		/// </summary>
		/// <param name="parent">親のワールド変換</param>
		void SetParent(const WorldTransform* parent) { parent_ = parent; }

	private:
		/// ============================== ///
		///		メンバ変数(private)
		/// ============================== ///

		// 定数バッファ(座標変換リソース)
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
		// マッピング済みアドレス
		WorldTransformationMatrixForVS* data_ = nullptr;

		// ローカルのスケール
		Vector3 scale_ = { 1, 1, 1 };
		// ローカルの回転角
		Vector3 rotate_ = { 0, 0, 0 };
		// ローカルの座標
		Vector3 translate_ = { 0, 0, 0 };
		//ワールド座標
		Vector3 worldTranslate_ = { 0, 0, 0 };
		//前フレームのワールド座標
		Vector3 preWorldTranslate_ = { 0,0,0 };

		// ローカル → ワールド変換行列
		Matrix4x4 matWorld_;
		// 親となるワールド変換へのポインタ
		const WorldTransform* parent_ = nullptr;

	};

	static_assert(!std::is_copy_assignable_v<WorldTransform>);

}