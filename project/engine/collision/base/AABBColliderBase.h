#pragma once
#include "ICollider.h"
#include <MyMath.h>

namespace Norm {

	/// <summary>
	/// AABB衝突判定用の基底クラス
	/// </summary>
	class AABBColliderBase :
		public ICollider {
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		AABBColliderBase();
		/// <summary>
		/// 仮想デストラクタ
		/// </summary>
		virtual ~AABBColliderBase() = default;

		/// <summary>
		/// デバッグ
		/// </summary>
		void Debug() override;

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// AABBを取得
		/// </summary>
		/// <returns></returns>
		const AABB& GetAABB() { return aabb_; }

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// AABBをセット
		/// </summary>
		/// <param name="_aabb">AABB</param>
		void SetAABB(const AABB& _aabb) { aabb_ = _aabb; }

	protected:
		/// ============================== ///
		///		メンバ変数(protected)
		/// ============================== ///

		//AABB用変数
		AABB aabb_ = {
			.min = {-1,-1,-1},
			.max = {1,1,1}
		};

	};

}