#pragma once
#include <vector>
#include <memory>
#include "ICollider.h"

namespace Norm {

	class SphereColliderBase;
	class AABBColliderBase;
	class OBBColliderBase;

	/// <summary>
	/// 全ての当たり判定処理を行うクラス
	/// シングルトンパターンで実装
	/// </summary>
	class CollisionManager {
	private://コンストラクタ等の隠蔽
		static std::unique_ptr<CollisionManager> instance_;

		CollisionManager() = default;//コンストラクタ隠蔽
		~CollisionManager() = default;//デストラクタ隠蔽
		CollisionManager(CollisionManager&) = delete;//コピーコンストラクタ封印
		CollisionManager& operator=(CollisionManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<CollisionManager>;

	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// インスタンス取得関数
		/// </summary>
		/// <returns></returns>
		static CollisionManager* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// 当たり判定チェック
		/// </summary>
		void CheckCollision();

		/// <summary>
		/// コライダーの登録
		/// </summary>
		/// <param name="collider">コライダーのポインタ</param>
		void RegistCollider(ICollider* _collider);
		/// <summary>
		/// コライダーの削除
		/// </summary>
		/// <param name="_collider">コライダーのポインタ</param>
		void DeleteCollider(ICollider* _collider);


	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// 当たり判定演算
		/// </summary>
		/// <param name="colliderA">コライダーAのポインタ</param>
		/// <param name="colliderB">コライダーBのポインタ</param>
		void CheckCollisionPair(ICollider* colliderA, ICollider* colliderB);

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		// コライダーリスト
		std::vector<ICollider*> colliders_;
	};

}