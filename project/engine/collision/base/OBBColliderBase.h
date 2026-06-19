#pragma once
#include "ICollider.h"

namespace Norm {

	/// <summary>
	/// OBB衝突判定用の基底クラス
	/// </summary>
	class OBBColliderBase :
		public ICollider {
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		OBBColliderBase();
		/// <summary>
		/// 仮想デストラクタ
		/// </summary>
		virtual ~OBBColliderBase() = default;

		/// <summary>
		/// デバッグ
		/// </summary>
		void Debug() override;

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// OBB（有向境界ボックス）を取得する
		/// </summary>
		/// <returns>OBB</returns>
		OBB GetOBB();

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// OBBのサイズを設定
		/// </summary>
		/// <param name="_size">OBBのサイズ</param>
		void SetOBBSize(const Vector3& _size) { size_ = _size; }

	protected:
		/// ============================== ///
		///		メンバ変数(protected)
		/// ============================== ///

		//OBB用変数
		Vector3 size_ = { 1,1,1 };

	};

}