#pragma once

namespace Norm {

	/// <summary>
	/// リソースの解放漏れをチェックするためのクラス
	/// </summary>
	class D3DResourceLeakChecker {
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// デストラクタ
		/// </summary>
		~D3DResourceLeakChecker();
	};

}