#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include "imgui.h"

namespace Norm {

	/// <summary>
	/// ImGuiを管理するクラス
	/// シングルトンパターンで実装
	/// </summary>
	class ImGuiManager {
	private://コンストラクタ等の隠蔽
		static std::unique_ptr<ImGuiManager> instance_;

		ImGuiManager() = default;//コンストラクタ隠蔽
		~ImGuiManager() = default;//デストラクタ隠蔽
		ImGuiManager(ImGuiManager&) = delete;//コピーコンストラクタ封印
		ImGuiManager& operator=(ImGuiManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<ImGuiManager>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		/// <returns>シングルトンインスタンス</returns>
		static ImGuiManager* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// ImGui受付開始
		/// </summary>
		void Begin();

		/// <summary>
		/// ImGui受付終了
		/// </summary>
		void End();

		/// <summary>
		/// 画面への描画
		/// </summary>
		void Draw();

	};

}	// namespace Norm

