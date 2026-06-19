#pragma once
#include <Object3d.h>
#include <WorldTransform.h>
#include <unordered_map>
#include <MyMath.h>
#include <string>
#include <cstdint>
#include <memory>

//アプリケーション
#include <application/object/level/collision/LevelObjectCollider.h>

/// <summary>
/// レベルオブジェクト単体基底クラス
/// </summary>
class IBaseLevelObject {
public:
	/// ============================== ///
	///		メンバ関数
	/// ============================== ///

	/// <summary>
	/// コンストラクタ
	/// </summary>
	IBaseLevelObject() = default;
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IBaseLevelObject() = default;
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_name">名前</param>
	virtual void Initialize(const std::string& _name);
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();
	/// <summary>
	/// ImGui操作
	/// </summary>
	virtual void DebugWithImGui();

	/// ============================== ///
	///		getter
	/// ============================== ///

	/// <summary>
	/// 名前の取得
	/// </summary>
	/// <returns>名前</returns>
	const std::string& GetName() const { return name_; }
	/// <summary>
	/// ワールド変換情報の取得
	/// </summary>
	/// <returns>ワールド変換情報</returns>
	const std::unordered_map<uint32_t, std::unique_ptr<Norm::WorldTransform>>& GetWorldTransforms() const { return worldTransforms_; }

	const std::unordered_map<uint32_t, std::unique_ptr<LevelObjectCollider>>& GetColliders() const { return colliders_; }
	/// <summary>
	/// オブジェクト3Dの取得
	/// </summary>
	/// <returns>オブジェクト3D</returns>
	Norm::Object3d* GetObject3d() { return object3d_.get(); }

	/// ============================== ///
	///		setter
	/// ============================== ///

	/// <summary>
	/// トランスフォーム情報の設定
	/// </summary>
	/// <param name="_transform">トランスフォーム情報</param>
	/// <returns>ハンドル</returns>
	uint32_t SetTransformInfo(const Norm::TransformEuler& _transform);

	/// <summary>
	/// コリジョン情報の設定
	/// </summary>
	/// <param name="_center">中心点</param>
	/// <param name="_size">サイズ</param>
	void SetCollisionInfo(uint32_t _handle, const Norm::Vector3& _center, const Norm::Vector3& _size);


protected:
	/// ============================== ///
	///		メンバ変数(protected)
	/// ============================== ///

	//テクスチャハンドル
	int32_t textureHandle_ = EOF;
	//オブジェクト3D
	std::unique_ptr<Norm::Object3d> object3d_ = nullptr;
	//ワールドトランスフォーム
	std::unordered_map<uint32_t, std::unique_ptr<Norm::WorldTransform>> worldTransforms_;
	//コライダー
	std::unordered_map<uint32_t, std::unique_ptr<LevelObjectCollider>> colliders_;

private:
	/// ============================== ///
	///		メンバ変数(private)
	/// ============================== ///

	//名前
	std::string name_;

};