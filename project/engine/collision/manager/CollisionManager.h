#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
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

		/// <summary>
		/// 空間分割グリッドの1セルあたりのサイズを変更する
		/// </summary>
		/// <param name="_cellSIze"></param>
	    void SetCellSize(float _cellSIze) { cellSize_ = _cellSIze; }

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
	    ///		空間分割関連
	    /// ============================== ///

		/// <summary>
		/// グリッドのセル座標
		/// </summary>
		struct GridCell {
		    int32_t x = 0;
		    int32_t y = 0;
		    int32_t z = 0;

			bool operator==(const GridCell& other) const { 
				return x == other.x && y == other.y && z == other.z;
			}

			// 隣接セルの重複処理を避けるための全順序比較
		    bool operator<(const GridCell& other) const {
			    if (x != other.x) return x < other.x;
			    if (y != other.y) return y < other.y;
			    return z < other.z;
			}
		};

		/// <summary>
		/// GridCellをunorderd_mapのキーとして使うためのハッシュ
		/// </summary>
		struct GridCellHash {
		    size_t operator()(const GridCell& cell) const { 
				size_t h1 = std::hash<int32_t>()(cell.x);
			    size_t h2 = std::hash<int32_t>()(cell.y);
			    size_t h3 = std::hash<int32_t>()(cell.z);
				// 空間ハッシュ
			    return h1 ^ (h2 * 73856093u) ^ (h3 * 19349663u);
			}
		};

		/// <summary>
		/// ワールド座標が属するグリッドセルを求める
		/// </summary>
		/// <param name="_position"></param>
		/// <returns></returns>
		GridCell WorldToCell(const Vector3& _position) const;

		/// <summary>
		/// 現在登録されているコライダーの位置を元にグリッドを再構築する
		/// </summary>
		void BuildGrid();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		// コライダーリスト
		std::vector<ICollider*> colliders_;

		// 空間分割グリッド
	    std::unordered_map<GridCell, std::vector<ICollider*>, GridCellHash> grid_;

		// グリッド1セルのサイズ
		float cellSize_ = 4.0f;
	};
}