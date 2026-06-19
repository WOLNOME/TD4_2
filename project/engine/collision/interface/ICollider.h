#pragma once
#include "CollisionConfig.h"
#include "WorldTransform.h"
#include "Vector3.h"
#include <string>

namespace Norm {

	/// ============================== ///
	///		列挙体
	/// ============================== ///

	//コライダーの形状
	enum class ColliderShape {
		Sphere,
		AABB,
		OBB,
	};

	/// <summary>
	/// 衝突判定のインターフェース
	/// </summary>
	class ICollider {
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		ICollider();
		/// <summary>
		/// 仮想デストラクタ
		/// </summary>
		virtual ~ICollider();

		/// <summary>
		/// デバッグ
		/// </summary>
		virtual void Debug() = 0;

		/// <summary>
		/// 衝突時のコールバック
		/// </summary>
		/// <param name="_other">衝突相手のポインタ</param>
		/// <param name="_attribute">衝突時の相手の属性</param>
		virtual void OnCollision([[maybe_unused]] ICollider* _other, CollisionAttribute _attribute) = 0;

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// シーンタグを取得する
		/// </summary>
		/// <returns>シーンタグ</returns>
		const std::string& GetSceneTag() { return sceneTag_; }

		/// <summary>
		/// コリジョン属性を取得する
		/// </summary>
		/// <returns>コリジョン判定に使用される属性値</returns>
		CollisionAttribute GetCollisionAttribute() { return collisionAttribute_; }

		/// <summary>
		/// コライダーの形状を取得する
		/// </summary>
		/// <returns>コライダーの形状</returns>
		ColliderShape GetColliderShape() { return colliderShape_; }

		/// <summary>
		/// このコライダーを保持者のポインタを取得する
		/// </summary>
		/// <returns>このコライダーを保持者のポインタ</returns>
		void* GetHolder() const { return holder_; }

		/// <summary>
		/// ワールドトランスフォームを取得する
		/// </summary>
		/// <returns>ワールドトランスフォームのポインタ</returns>
		const WorldTransform& GetWorldTransform() { return *worldTransform_; }

		/// <summary>
		/// 中心座標の取得
		/// </summary>
		/// <returns>中心座標</returns>
		Vector3 GetCenter();

		/// <summary>
		/// オフセットの取得
		/// </summary>
		/// <returns>オフセット</returns>
		const Vector3& GetOffset() { return offset_; }

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// コリジョン属性を設定する
		/// </summary>
		/// <param name="collisionAttribute">設定するコリジョン属性</param>
		void SetCollisionAttribute(const CollisionAttribute collisionAttribute) { collisionAttribute_ = collisionAttribute; }

		/// <summary>
		/// このコライダーの保持者をセット
		/// </summary>
		/// <param name="_holder">このコライダーの保持者</param>
		void SetHolder(void* _holder) { holder_ = _holder; }

		/// <summary>
		/// ワールドトランスフォームを設定する
		/// </summary>
		/// <param name="_worldTransform">設定するワールドトランスフォームのポインタ</param>
		void SetWorldTransform(WorldTransform* _worldTransform) { worldTransform_ = _worldTransform; }

		/// <summary>
		/// オフセットを設定する
		/// </summary>
		/// <param name="_offset">設定するオフセット</param>
		void SetOffset(const Vector3& _offset) { offset_ = _offset; }

	protected:
		/// ============================== ///
		///		メンバ変数(protected)
		/// ============================== ///

		//シーンタグ
		std::string sceneTag_ = "";

		//コリジョン属性
		CollisionAttribute collisionAttribute_ = CollisionAttribute::Nothingness;

		//コライダーの形状
		ColliderShape colliderShape_;

		//このコライダーの保持者のポインタ
		void* holder_ = nullptr;

		//対象のワールドトランスフォーム
		WorldTransform* worldTransform_ = nullptr;

		//中心座標
		Vector3 center_ = { 0,0,0 };
		//オフセット
		Vector3 offset_ = { 0,0,0 };

		//デバッグ用変数
		Vector4 debugLineColor_ = { 1,1,1,1 };

	};

}