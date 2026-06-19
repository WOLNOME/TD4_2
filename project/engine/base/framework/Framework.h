#pragma once
#include "D3DResourceLeakChecker.h"
#include <memory>

namespace Norm {

	/// <summary>
	/// プロジェクト全体のフレームワーク
	/// </summary>
	class Framework {
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~Framework() = default;
		/// <summary>
		/// 初期化
		/// </summary>
		virtual void Initialize();
		/// <summary>
		/// 終了時
		/// </summary>
		virtual void Finalize();
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update();
		/// <summary>
		/// 描画
		/// </summary>
		virtual void Draw() = 0;
		/// <summary>
		/// 終了判定
		/// </summary>
		/// <returns></returns>
		virtual bool GetOver() { return isOver; }

		/// <summary>
		/// 実行
		/// </summary>
		void Run();

	protected:
		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		D3DResourceLeakChecker leakChecker;
		bool isOver = false;

	};

}